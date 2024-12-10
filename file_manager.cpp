#include "file_manager.h"

// Constructor
FileManager::FileManager(DiskManager& diskManager) 
    : diskManager(diskManager), fileTable() {}

// Initialize Root Directory
void FileManager::initializeFileSystem() {
    try {
        //  Ensure the root directory '/' exists
        const FileEntry* rootEntry = findEntry("/");
        if (!rootEntry) {
            FileEntry root("/", FileType::Directory, 0, {});
            fileTable.addEntry(root);
        } else {}
    } catch (const std::exception& e) {
        std::cerr << "Error creating root directory: " << e.what() << std::endl;
    }
}

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
    if (path == "/") return "/";
    
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

bool isNumber(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

// file_manager.cpp

// Ensure that all parent directories exist for a given path
void FileManager::ensureParentDirectories(const std::string& path) {
    // Tokenize the path into its components
    std::vector<std::string> tokens = tokenizePath(path);

    std::string currentPath = "/";
    for (size_t i = 0; i < tokens.size() - 1; ++i) { // Exclude the file or last directory name
        currentPath += tokens[i];
        const FileEntry* entry = findEntry(currentPath);

        // If the directory does not exist, create it
        if (!entry) {
            createDirectory(currentPath);
        } else if (entry->type != FileType::Directory) {
            throw std::runtime_error("Path conflict: " + currentPath + " exists but is not a directory.");
        }

        // Move to the next level
        currentPath += "/";
    }
}

void FileManager::createFile(const std::string& path, size_t size) {
    if (!isValidName(path)) throw std::invalid_argument("Invalid file name.");

    std::string resolvedPath = resolvePath(path);

    // Ensure parent directories exist
    ensureParentDirectories(resolvedPath);

    if (findEntry(resolvedPath)) throw std::runtime_error("File already exists.");

    // Allocate required blocks
    size_t numBlocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    std::vector<size_t> blocks;
    for (size_t i = 0; i < numBlocks; ++i) {
        size_t blockIndex = diskManager.allocateBlock();
        blocks.push_back(blockIndex);
    }

    // Create the file entry
    FileEntry entry(resolvedPath, FileType::File, size, blocks);
    fileTable.addEntry(entry);
}

void FileManager::createDirectory(const std::string& path) {
    if (!isValidName(path)) throw std::invalid_argument("Invalid directory name.");

    auto resolvedPath = resolvePath(path);

    // Ensure parent directories exist
    ensureParentDirectories(resolvedPath);

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

std::vector<std::string> FileManager::listDirectory(const std::string& path) const {
    const auto* entry = findEntry(path);

    if (!entry) throw std::runtime_error("Directory does not exist.");
    if (entry->type != FileType::Directory) throw std::runtime_error("Path is not a directory.");

    std::vector<std::string> contents;

    // Ensure path always ends with a '/'
    std::string basePath = path.back() == '/' ? path : path + '/';

    for (const auto& [name, subEntry] : fileTable.getEntries()) {
        if (name.find(basePath) == 0 && name != path) {
            std::string relativePath = name.substr(basePath.size());
            // Extract the first sub-directory or file from the relative path
            size_t nextSlash = relativePath.find('/');
            
            //  If nextSlash is found, we have a directory or subdirectory (e.g., /folder/file.txt)
            if (nextSlash != std::string::npos) {
                relativePath = relativePath.substr(0, nextSlash);
            }

            //  Avoid duplicate entries
            if (!relativePath.empty() && 
                std::find(contents.begin(), contents.end(), relativePath) == contents.end()) {
                contents.push_back(relativePath);
            }
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


void FileManager::save(std::ofstream& outFile) const {
    const auto& entries = fileTable.getEntries();
    size_t fileCount = entries.size();
    outFile.write(reinterpret_cast<const char*>(&fileCount), sizeof(fileCount));

    for (const auto& [path, entry] : entries) {
        size_t pathLength = path.size();
        outFile.write(reinterpret_cast<const char*>(&pathLength), sizeof(pathLength));
        outFile.write(path.c_str(), pathLength);
        
        // Save the FileType (whether it's a File or Directory)
        outFile.write(reinterpret_cast<const char*>(&entry.type), sizeof(entry.type));

        outFile.write(reinterpret_cast<const char*>(&entry.size), sizeof(entry.size));

        size_t blockCount = entry.blockIndices.size();
        outFile.write(reinterpret_cast<const char*>(&blockCount), sizeof(blockCount));
        for (size_t blockIndex : entry.blockIndices) {
            outFile.write(reinterpret_cast<const char*>(&blockIndex), sizeof(blockIndex));
        }
    }

    diskManager.save(outFile);
}

void FileManager::load(std::ifstream& inFile) {
    size_t fileCount;
    inFile.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));

    for (size_t i = 0; i < fileCount; ++i) {
        size_t pathLength;
        inFile.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));

        std::string path(pathLength, '\0');
        inFile.read(&path[0], pathLength);

        // Read FileType (FileType::File or FileType::Directory)
        FileType type;
        inFile.read(reinterpret_cast<char*>(&type), sizeof(type));

        size_t size;
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size));

        std::vector<size_t> blocks;
        size_t blockCount;
        inFile.read(reinterpret_cast<char*>(&blockCount), sizeof(blockCount));
        for (size_t j = 0; j < blockCount; ++j) {
            size_t blockIndex;
            inFile.read(reinterpret_cast<char*>(&blockIndex), sizeof(blockIndex));
            blocks.push_back(blockIndex);
        }

        FileEntry entry(path, type, size, blocks);
        fileTable.addEntry(entry);
    }

    // Ensure / exists and is a directory
    const FileEntry* rootEntry = fileTable.getEntry("/");
    if (!rootEntry || rootEntry->type != FileType::Directory) {
        if (rootEntry) fileTable.removeEntry("/");
        FileEntry root("/", FileType::Directory, 0, {});
        fileTable.addEntry(root);
    }

    //  Load DiskManager state
    diskManager.load(inFile);

}

void FileManager::moveFile(const std::string& sourcePath, const std::string& destinationPath) {
    std::string srcResolved = resolvePath(sourcePath);
    std::string destResolved = resolvePath(destinationPath);

    if (srcResolved == destResolved) {
        throw std::runtime_error("Source and destination paths are the same.");
    }

    const FileEntry* sourceEntry = findEntry(srcResolved);
    if (!sourceEntry) {
        throw std::runtime_error("Source file does not exist: " + srcResolved);
    }

    if (sourceEntry->type != FileType::File) {
        throw std::runtime_error("Source is not a file: " + srcResolved);
    }

    //  Check if the parent directory exists before creating it
    std::string parentPath = resolvePath(destResolved.substr(0, destResolved.find_last_of('/')));
    const FileEntry* parentEntry = findEntry(parentPath);
    if (!parentEntry) {
        throw std::runtime_error("Parent directory does not exist: " + parentPath);
    }

    // Read the file data from the source path
    std::string fileData = readFile(srcResolved);

    // Create a new file at the destination and write the data
    createFile(destResolved, sourceEntry->size);
    writeFile(destResolved, fileData, false); // Write all the file data

    // Remove source entry from file table
    fileTable.removeEntry(srcResolved);
}


void FileManager::moveDirectory(const std::string& sourcePath, const std::string& destinationPath) {
    std::string srcResolved = resolvePath(sourcePath);
    std::string destResolved = resolvePath(destinationPath);

    if (srcResolved == destResolved) {
        throw std::runtime_error("Source and destination paths are the same.");
    }

    if (destinationPath.find(sourcePath) == 0) {
        throw std::runtime_error("Cannot move a directory into itself: " + destinationPath);
    }

    const FileEntry* sourceEntry = findEntry(srcResolved);
    if (!sourceEntry) {
        throw std::runtime_error("Source directory does not exist: " + srcResolved);
    }

    if (sourceEntry->type != FileType::Directory) {
        throw std::runtime_error("Source is not a directory: " + srcResolved);
    }

    //  Create the destination directory entry before moving contents
    FileEntry newDirEntry = *sourceEntry;
    newDirEntry.name = destResolved;
    fileTable.addEntry(newDirEntry); // Add the new directory entry before moving contents

    //  Recursively move subdirectories and files
    std::vector<std::string> contents = listDirectory(srcResolved);
    for (const std::string& item : contents) {
        std::string srcItemPath = srcResolved + "/" + item;
        std::string destItemPath = destResolved + "/" + item;

        const FileEntry* entry = findEntry(srcItemPath);
        if (!entry) continue;

        if (entry->type == FileType::File) {
            moveFile(srcItemPath, destItemPath);
        } else if (entry->type == FileType::Directory) {
            moveDirectory(srcItemPath, destItemPath);
        }
    }

    //  Remove the source directory from fileTable
    deleteFile(srcResolved);
}

void FileManager::renamePath(const std::string& sourcePath, const std::string& newName) {
    std::string srcResolved = resolvePath(sourcePath);
    const FileEntry* sourceEntry = findEntry(srcResolved);
    if (!sourceEntry) {
        throw std::runtime_error("Source path does not exist: " + srcResolved);
    }

    std::string parentPath = srcResolved.substr(0, srcResolved.find_last_of('/'));
    std::string destResolved = parentPath + "/" + newName;

    if (findEntry(destResolved)) {
        throw std::runtime_error("File or directory with the new name already exists: " + destResolved);
    }

    fileTable.removeEntry(srcResolved);
    FileEntry newEntry = *sourceEntry;
    newEntry.name = destResolved;
    fileTable.addEntry(newEntry);

    //std::cout << "Renamed " << sourcePath << " to " << newName << std::endl;
}