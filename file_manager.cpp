#include "file_manager.h"
#include <stdexcept>

// FileEntry Implementation
FileEntry::FileEntry(std::string name, FileType type, size_t size, const std::vector<size_t>& blocks)
    : name(std::move(name)), type(type), size(size), blockIndices(blocks) {}

// FileTable Implementation
void FileTable::addEntry(const FileEntry& entry) {
    if (entries.find(entry.name) != entries.end())
        throw std::runtime_error("File entry already exists");
    entries[entry.name] = entry;
}

bool FileTable::removeEntry(const std::string& name) {
    return entries.erase(name) > 0;
}

const FileEntry* FileTable::getEntry(const std::string& name) const {
    auto it = entries.find(name);
    return it != entries.end() ? &it->second : nullptr;
}

const std::unordered_map<std::string, FileEntry>& FileTable::getEntries() const {
    return entries;
}

// FileManager Implementation
FileManager::FileManager(DiskManager& diskManager)
    : diskManager(diskManager) {}

void FileManager::createFile(const std::string& path, size_t size) {
    if (fileTable.getEntry(path)) throw std::runtime_error("File already exists");

    size_t numBlocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE; // Calculate required blocks
    std::vector<size_t> allocatedBlocks;

    for (size_t i = 0; i < numBlocks; ++i) {
        allocatedBlocks.push_back(diskManager.allocateBlock());
    }

    fileTable.addEntry(FileEntry(path, FileType::File, size, allocatedBlocks));
}

void FileManager::createDirectory(const std::string& path) {
    if (fileTable.getEntry(path)) throw std::runtime_error("Directory already exists");
    fileTable.addEntry(FileEntry(path, FileType::Directory, 0, {}));
}

std::vector<std::string> FileManager::listDirectory(const std::string& path) {
    const auto* dir = fileTable.getEntry(path);
    if (!dir || dir->type != FileType::Directory)
        throw std::runtime_error("Invalid directory path");

    std::vector<std::string> result;
    for (const auto& [name, entry] : fileTable.getEntries()) {
        if (name.find(path) == 0) result.push_back(name);
    }
    return result;
}
