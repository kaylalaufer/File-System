#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <cstring>

// Constants
const size_t MAX_BLOCKS = 256;
const size_t BLOCK_SIZE = 4096;

// Bitmap class: Manages free/occupied blocks
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

// DiskManager: Handles block-level operations
class DiskManager {
public:
    DiskManager(const std::string& diskName, size_t numBlocks);

    void writeBlock(size_t blockIndex, const std::string& data);
    std::string readBlock(size_t blockIndex) const;
    void deleteBlock(size_t blockIndex);

    size_t allocateBlock();
    void setBlockFree(size_t blockIndex);

    const Bitmap& getBitmap() const;

    void save(std::ofstream& outFile) const;
    void load(std::ifstream& inFile);

private:
    std::fstream diskFile;
    std::string diskName;
    size_t numBlocks;
    Bitmap bitmap;
};

#endif // DISK_MANAGER_H
