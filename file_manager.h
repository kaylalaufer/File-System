#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "disk_manager.h"
#include <string>
#include <vector>
#include <unordered_map>

// Enum for File Type
enum class FileType {
    File,
    Directory
};

// FileEntry struct
struct FileEntry {
    std::string name;
    FileType type;
    size_t size;
    std::vector<size_t> blockIndices;

    FileEntry(std::string name, FileType type, size_t size, const std::vector<size_t>& blocks);
};

// FileTable class
class FileTable {
public:
    void addEntry(const FileEntry& entry);
    bool removeEntry(const std::string& name);
    const FileEntry* getEntry(const std::string& name) const;
    const std::unordered_map<std::string, FileEntry>& getEntries() const;

private:
    std::unordered_map<std::string, FileEntry> entries;
};

// FileManager: Handles files and directories
class FileManager {
public:
    FileManager(DiskManager& diskManager);

    void createFile(const std::string& path, size_t size);
    void createDirectory(const std::string& path);
    std::vector<std::string> listDirectory(const std::string& path);

private:
    DiskManager& diskManager;
    FileTable fileTable;

    void validatePath(const std::string& path) const;
};

#endif // FILE_MANAGER_H
