#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>

// Constants
const size_t MAX_BLOCKS = 256;
const size_t BLOCK_SIZE = 4096;

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

// Bitmap class
class Bitmap {
public:
    explicit Bitmap(size_t numBlocks);

    bool isFree(size_t blockIndex) const;
    void setOccupied(size_t blockIndex);
    void setFree(size_t blockIndex);

    const std::vector<bool>& getBitmap() const;

private:
    std::vector<bool> bitmap;
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

// DiskManager class
class DiskManager {
public:
    DiskManager(const std::string& diskName, size_t numBlocks);
    void writeBlock(size_t blockIndex, const std::string& data);
    std::string readBlock(size_t blockIndex) const;
    void deleteBlock(size_t blockIndex);
    void saveToDisk() const;
    void loadFromDisk();

    const Bitmap& getBitmap() const;
    FileTable& getFileTable();

private:
    std::string diskName;
    size_t numBlocks;
    Bitmap bitmap;
    FileTable fileTable;
};

#endif // DISK_MANAGER_H
