#include "disk_manager.h"


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
    : diskName(diskName), numBlocks(numBlocks), bitmap(numBlocks) {
    // 🔥 Open the disk file for reading and writing
    diskFile.open(diskName, std::ios::in | std::ios::out | std::ios::binary);
    
    if (!diskFile) {
        // 🔥 If the disk file doesn't exist, create it
        diskFile.clear();
        diskFile.open(diskName, std::ios::out | std::ios::binary); // Create file
        diskFile.close();
        diskFile.open(diskName, std::ios::in | std::ios::out | std::ios::binary); // Re-open in read/write mode

        // 🔥 Initialize the disk with empty blocks
        std::string emptyBlock(BLOCK_SIZE, '\0');
        for (size_t i = 0; i < numBlocks; ++i) {
            diskFile.write(emptyBlock.c_str(), BLOCK_SIZE);
        }
    }

    if (!diskFile) {
        throw std::runtime_error("Failed to open disk file for read/write");
    }
}

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


void DiskManager::save(std::ofstream& outFile) const {
    // Save the bitmap
    size_t bitmapSize = bitmap.getBitmap().size();
    outFile.write(reinterpret_cast<const char*>(&bitmapSize), sizeof(bitmapSize));

    // 🔥 Convert bool to char and write
    for (bool bit : bitmap.getBitmap()) {
        char byte = bit ? 1 : 0; // Convert bool to byte
        outFile.write(&byte, sizeof(char));
    }

    // Save all block data
    for (size_t i = 0; i < numBlocks; ++i) {
        if (!bitmap.isFree(i)) {
            std::string blockData = readBlock(i); // Read the content of the block
            size_t dataSize = blockData.size();
            outFile.write(reinterpret_cast<const char*>(&i), sizeof(i)); // Save the block index
            outFile.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize)); // Save size of block data
            outFile.write(blockData.c_str(), dataSize); // Write the actual block data
        }
    }

    // Write an end marker to know we've written all blocks
    size_t endMarker = static_cast<size_t>(-1);
    outFile.write(reinterpret_cast<const char*>(&endMarker), sizeof(endMarker));
}


void DiskManager::load(std::ifstream& inFile) {
    // Load the bitmap
    size_t bitmapSize;
    inFile.read(reinterpret_cast<char*>(&bitmapSize), sizeof(bitmapSize));
    if (inFile.eof()) return; // 🔥 Early exit if EOF reached

    std::vector<bool> bitmapData(bitmapSize);
    for (size_t i = 0; i < bitmapSize; ++i) {
        char byte;
        inFile.read(&byte, sizeof(char));
        if (inFile.eof()) return; // 🔥 Early exit if EOF reached
        bitmapData[i] = (byte == 1); // Convert byte back to bool
    }

    bitmap = Bitmap(bitmapSize);
    for (size_t i = 0; i < bitmapSize; ++i) {
        if (!bitmapData[i]) {
            bitmap.setOccupied(i);
        }
    }

    // Load all block data
    while (true) {
        size_t blockIndex;
        inFile.read(reinterpret_cast<char*>(&blockIndex), sizeof(blockIndex));
        if (inFile.eof()) break; // 🔥 Check for EOF before using blockIndex
        if (blockIndex == static_cast<size_t>(-1)) break; // 🔥 Check for end marker
        if (blockIndex >= numBlocks) {
            throw std::runtime_error("Invalid block index read from file");
        }

        size_t dataSize;
        inFile.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
        if (inFile.eof()) break; // 🔥 Check for EOF
        if (dataSize > BLOCK_SIZE) {
            throw std::runtime_error("Data size exceeds block size");
        }

        std::string blockData(dataSize, '\0');
        inFile.read(&blockData[0], dataSize);
        if (inFile.eof()) break; // 🔥 Check for EOF

        writeBlock(blockIndex, blockData); // Write the data back to the block
    }
}



