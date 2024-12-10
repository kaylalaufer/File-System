#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "disk_manager.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <sstream>
#include <iostream>
#include <fstream>


// Utility function to check if a string is a number
bool isNumber(const std::string& str);

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

    // Default constructor
    FileEntry() : name(""), type(FileType::File), size(0), blockIndices() {}

    // Custom constructor
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
    explicit FileManager(DiskManager& diskManager);

    void initializeFileSystem();

    void createFile(const std::string& path, size_t size);
    void createDirectory(const std::string& path);
    void ensureParentDirectories(const std::string& path);
    void deleteFile(const std::string& path);
    void deleteDirectory(const std::string& path, bool recursive = false);
    std::vector<std::string> listDirectory(const std::string& path) const;
    const FileEntry* getMetadata(const std::string& path) const;
    void writeFile(const std::string& path, const std::string& data, bool append);
    std::string readFile(const std::string& path) const;
    void openFile(const std::string& path) const;

    void save(std::ofstream& outFile) const; // Save file system state
    void load(std::ifstream& inFile);        // Load file system state

private:
    DiskManager& diskManager;
    FileTable fileTable;

    bool isValidName(const std::string& name) const;
    std::vector<std::string> tokenizePath(const std::string& path) const;
    std::string resolvePath(const std::string& path) const;
    const FileEntry* findEntry(const std::string& path) const;
};

#endif // FILE_MANAGER_H
