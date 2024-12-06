#include "file_manager.h"

// Constructor
FileManager::FileManager(DiskManager& diskManager) 
    : diskManager(diskManager), fileTable() {}

FileEntry::FileEntry(std::string name, FileType type, size_t size, const std::vector<size_t>& blocks)
    : name(std::move(name)), type(type), size(size), blockIndices(blocks) {}

void FileTable::addEntry(const FileEntry& entry) {
    if (entries.find(entry.name) != entries.end()) {
        throw std::runtime_error("File entry already exists");
    }
    entries[entry.name] = entry;
}

bool FileTable::removeEntry(const std::string& name) {
    return entries.erase(name) > 0;
}

const FileEntry* FileTable::getEntry(const std::string& name) const {
    auto it = entries.find(name);
    if (it == entries.end()) {
        return nullptr;
    }
    return &it->second;
}

const std::unordered_map<std::string, FileEntry>& FileTable::getEntries() const {
    return entries;
}


// Utility: Validate names
bool FileManager::isValidName(const std::string& name) const {
    std::regex validNameRegex("^[a-zA-Z0-9_./-]+$"); // Allow `/` for paths
    return !name.empty() && std::regex_match(name, validNameRegex);
}


// Utility: Split path into components
std::vector<std::string> FileManager::tokenizePath(const std::string& path) const {
    std::vector<std::string> tokens;
    std::istringstream stream(path);
    std::string token;

    while (std::getline(stream, token, '/')) {
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

// Utility: Resolve absolute paths
std::string FileManager::resolvePath(const std::string& path) const {
    std::vector<std::string> tokens = tokenizePath(path);
    std::vector<std::string> resolved;

    for (const auto& token : tokens) {
        if (token == ".." && !resolved.empty()) {
            resolved.pop_back();
        } else if (token != "." && token != "..") {
            resolved.push_back(token);
        }
    }

    std::string resolvedPath = "/";
    for (const auto& part : resolved) {
        resolvedPath += part + "/";
    }
    return resolvedPath.empty() ? "/" : resolvedPath.substr(0, resolvedPath.length() - 1);
}

// Utility: Find a file/directory entry
const FileEntry* FileManager::findEntry(const std::string& path) const {
    auto resolvedPath = resolvePath(path);
    return fileTable.getEntry(resolvedPath);
}

// Create a file
void FileManager::createFile(const std::string& path, size_t size) {
    if (!isValidName(path)) throw std::invalid_argument("Invalid file name.");

    std::string resolvedPath = resolvePath(path);

    // Debug: Check FileTable before creation
    std::cout << "FileTable entries before creating file:" << std::endl;
    for (const auto& [name, entry] : fileTable.getEntries()) {
        std::cout << "  " << name << std::endl;
    }

    if (findEntry(resolvedPath)) throw std::runtime_error("File already exists.");

    // Allocate required blocks
    size_t numBlocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    std::vector<size_t> blocks;
    for (size_t i = 0; i < numBlocks; ++i) {
        size_t blockIndex = diskManager.allocateBlock();
        blocks.push_back(blockIndex);
    }

    // Debug: Print block allocations
    std::cout << "Allocated blocks: ";
    for (size_t block : blocks) {
        std::cout << block << " ";
    }
    std::cout << std::endl;

    // Create the file entry
    FileEntry entry(resolvedPath, FileType::File, size, blocks);
    fileTable.addEntry(entry);

    // Debug: Check FileTable after creation
    std::cout << "FileTable entries after creating file:" << std::endl;
    for (const auto& [name, entry] : fileTable.getEntries()) {
        std::cout << "  " << name << std::endl;
    }

    std::cout << "Allocated blocks: ";
    for (size_t block : blocks) {
        std::cout << block << " ";
    }
    std::cout << std::endl; // Should this print nothing??
}

// Create a directory
void FileManager::createDirectory(const std::string& path) {
    if (!isValidName(path)) throw std::invalid_argument("Invalid directory name.");

    auto resolvedPath = resolvePath(path);
    if (findEntry(resolvedPath)) throw std::runtime_error("Directory already exists.");

    // Create directory entry with zero blocks
    FileEntry entry(resolvedPath, FileType::Directory, 0, {});
    fileTable.addEntry(entry);
}

// Delete a file
void FileManager::deleteFile(const std::string& path) {
    const auto* entry = findEntry(path);
    if (!entry) throw std::runtime_error("File does not exist.");
    if (entry->type != FileType::File) throw std::runtime_error("Path is not a file.");

    // Free associated blocks
    for (size_t blockIndex : entry->blockIndices) {
        diskManager.deleteBlock(blockIndex);
    }

    fileTable.removeEntry(path);
}

// Delete a directory
void FileManager::deleteDirectory(const std::string& path, bool recursive) {
    const auto* entry = findEntry(path);
    if (!entry) throw std::runtime_error("Directory does not exist.");
    if (entry->type != FileType::Directory) throw std::runtime_error("Path is not a directory.");

    for (const auto& [name, subEntry] : fileTable.getEntries()) { // Use local fileTable
        if (name.find(path) == 0 && name != path) {
            if (!recursive) throw std::runtime_error("Directory is not empty.");
            if (subEntry.type == FileType::File) {
                deleteFile(name);
            } else {
                deleteDirectory(name, true);
            }
        }
    }

    fileTable.removeEntry(path); // Use local fileTable
}

// List directory contents
std::vector<std::string> FileManager::listDirectory(const std::string& path) const {
    const auto* entry = findEntry(path);
    if (!entry) throw std::runtime_error("Directory does not exist.");
    if (entry->type != FileType::Directory) throw std::runtime_error("Path is not a directory.");

    std::vector<std::string> contents;
    for (const auto& [name, subEntry] : fileTable.getEntries()) { // Use local fileTable
        if (name.find(path) == 0 && name != path) {
            contents.push_back(name);
        }
    }
    return contents;
}

// Get metadata
const FileEntry* FileManager::getMetadata(const std::string& path) const {
    return findEntry(path);
}

// Write data to a file
void FileManager::writeFile(const std::string& path, const std::string& data, bool append) {
    const auto* entry = findEntry(path);
    if (!entry) throw std::runtime_error("File does not exist.");
    if (entry->type != FileType::File) throw std::runtime_error("Path is not a file.");

    // Calculate the total data size after the write
    std::string fileData = append ? readFile(path) : "";
    fileData += data;

    // Ensure data fits within the file's maximum block capacity
    if (fileData.size() > MAX_BLOCKS * BLOCK_SIZE) {
        throw std::runtime_error("File exceeds maximum size");
    }

    size_t newSize = fileData.size();
    size_t requiredBlocks = (newSize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Allocate or reuse blocks
    std::vector<size_t> newBlocks;
    try {
        for (size_t i = 0; i < requiredBlocks; ++i) {
            if (i < entry->blockIndices.size()) {
                newBlocks.push_back(entry->blockIndices[i]); // Reuse existing blocks
            } else {
                newBlocks.push_back(diskManager.allocateBlock()); // Allocate new blocks
            }
        }

        // Write data block by block
        for (size_t i = 0; i < requiredBlocks; ++i) {
            size_t blockIndex = newBlocks[i];
            std::string blockData = fileData.substr(i * BLOCK_SIZE, BLOCK_SIZE);
            diskManager.writeBlock(blockIndex, blockData);
        }

        // Free any extra blocks
        for (size_t i = requiredBlocks; i < entry->blockIndices.size(); ++i) {
            diskManager.setBlockFree(entry->blockIndices[i]);
        }

        // Update metadata
        FileEntry updatedEntry = *entry;
        updatedEntry.size = newSize;
        updatedEntry.blockIndices = newBlocks;

        // Replace the old entry
        fileTable.removeEntry(updatedEntry.name);
        fileTable.addEntry(updatedEntry);
    } catch (const std::exception& e) {
        // Rollback newly allocated blocks in case of an error
        for (size_t i = entry->blockIndices.size(); i < newBlocks.size(); ++i) {
            diskManager.setBlockFree(newBlocks[i]);
        }
        throw; // Re-throw the exception
    }
}


// Read data from a file
std::string FileManager::readFile(const std::string& path) const {
    const auto* entry = findEntry(path);
    if (!entry) throw std::runtime_error("File does not exist.");
    if (entry->type != FileType::File) throw std::runtime_error("Path is not a file.");

    // Read the data block by block
    std::string data;
    for (size_t blockIndex : entry->blockIndices) {
        data += diskManager.readBlock(blockIndex);
        // std::cout << "hi" << data << std::endl;
    }

    // Trim the data to the file's actual size
    data.resize(entry->size);
    return data;
}

void FileManager::openFile(const std::string& path) const {
    const auto* entry = findEntry(path);
    if (!entry) throw std::runtime_error("File does not exist.");
    if (entry->type != FileType::File) throw std::runtime_error("Path is not a file.");

    std::string content = readFile(path);
    if (content.empty()) {
        std::cout << "File is empty." << std::endl;
    } else {
        std::cout << "Contents of " << path << ":\n" << content << std::endl;
    }
}
