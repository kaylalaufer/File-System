#include "disk_manager.h"
#include <fstream>
#include <stdexcept>
#include <cstring>

// Bitmap Implementation
Bitmap::Bitmap(size_t numBlocks) : bitmap(numBlocks, true) {}

bool Bitmap::isFree(size_t blockIndex) const {
    if (blockIndex >= bitmap.size()) throw std::out_of_range("Block index out of range");
    return bitmap[blockIndex];
}

void Bitmap::setOccupied(size_t blockIndex) {
    if (blockIndex >= bitmap.size()) throw std::out_of_range("Block index out of range");
    bitmap[blockIndex] = false;
}

void Bitmap::setFree(size_t blockIndex) {
    if (blockIndex >= bitmap.size()) throw std::out_of_range("Block index out of range");
    bitmap[blockIndex] = true;
}

const std::vector<bool>& Bitmap::getBitmap() const {
    return bitmap;
}

// DiskManager Implementation
DiskManager::DiskManager(const std::string& diskName, size_t numBlocks)
    : diskName(diskName), numBlocks(numBlocks), bitmap(numBlocks) {}

void DiskManager::writeBlock(size_t blockIndex, const std::string& data) {
    if (data.size() > BLOCK_SIZE) {
        throw std::runtime_error("Data size exceeds block size");
    }

    if (blockIndex >= numBlocks) {
        throw std::out_of_range("Block index out of range");
    }

    // Open disk file
    std::fstream disk(diskName, std::ios::in | std::ios::out | std::ios::binary);
    if (!disk) {
        throw std::runtime_error("Failed to open disk file for writing");
    }

    // Write the data, padded to BLOCK_SIZE
    std::string paddedData = data;
    paddedData.resize(BLOCK_SIZE, '\0'); // Pad with null bytes
    disk.seekp(blockIndex * BLOCK_SIZE);
    disk.write(paddedData.c_str(), BLOCK_SIZE);
    disk.close();

    // Update bitmap
    bitmap.setOccupied(blockIndex);
}

std::string DiskManager::readBlock(size_t blockIndex) const {
    if (blockIndex >= numBlocks) {
        throw std::out_of_range("Block index out of range");
    }

    if (bitmap.isFree(blockIndex)) {
        throw std::runtime_error("Block is free and contains no data");
    }

    std::ifstream disk(diskName, std::ios::binary);
    if (!disk) {
        throw std::runtime_error("Failed to open disk file for reading");
    }

    // Read the block data
    disk.seekg(blockIndex * BLOCK_SIZE);
    std::string data(BLOCK_SIZE, '\0');
    disk.read(&data[0], BLOCK_SIZE);
    disk.close();

    // Trim trailing null bytes
    size_t dataEnd = data.find_last_not_of('\0');
    if (dataEnd != std::string::npos) {
        data.resize(dataEnd + 1);
    } else {
        data.clear(); // Data is entirely null bytes
    }

    return data;
}

void DiskManager::deleteBlock(size_t blockIndex) {
    if (blockIndex >= numBlocks) {
        throw std::out_of_range("Block index out of range");
    }

    if (bitmap.isFree(blockIndex)) {
        throw std::runtime_error("Block is already free");
    }

    // Ensure the disk file exists
    std::fstream disk(diskName, std::ios::in | std::ios::out | std::ios::binary);
    if (!disk) {
        // Create the disk file if it doesn't exist
        std::ofstream createDisk(diskName, std::ios::binary | std::ios::trunc);
        createDisk.close();
        disk.open(diskName, std::ios::in | std::ios::out | std::ios::binary);
        if (!disk) {
            throw std::runtime_error("Failed to open disk file for deleting");
        }
    }

    // Overwrite the block with empty data
    std::string emptyData(BLOCK_SIZE, '\0');
    disk.seekp(blockIndex * BLOCK_SIZE);
    disk.write(emptyData.c_str(), BLOCK_SIZE);
    disk.close();

    // Update bitmap
    bitmap.setFree(blockIndex);
}

size_t DiskManager::allocateBlock() {
    for (size_t i = 0; i < numBlocks; ++i) {
        if (bitmap.isFree(i)) {
            bitmap.setOccupied(i);
            return i;
        }
    }
    throw std::runtime_error("No free blocks available");
}

void DiskManager::setBlockFree(size_t blockIndex) {
    bitmap.setFree(blockIndex);
}

const Bitmap& DiskManager::getBitmap() const {
    return bitmap;
}
