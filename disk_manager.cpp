#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <iomanip>
#include "disk_manager.h"

const size_t BLOCK_SIZE = 4096; // Define the size of each block (e.g., 4 KB)

void initializeDisk(const std::string& diskName, size_t diskSize) {
    std::fstream diskFile;

    // Try opening the file
    diskFile.open(diskName, std::ios::in | std::ios::out | std::ios::binary);
    if (!diskFile) {
        // File does not exist, so create and initialize it
        std::cout << "Disk file does not exist. Creating a new one..." << std::endl;
        diskFile.open(diskName, std::ios::out | std::ios::binary);
        if (!diskFile) {
            throw std::runtime_error("Failed to create disk file.");
        }
        // Write zeros to the file to initialize it
        std::vector<char> zeros(diskSize, 0);
        diskFile.write(zeros.data(), diskSize);
        diskFile.close();
        std::cout << "Disk initialized with size: " << diskSize << " bytes." << std::endl;
    } else {
        // File exists, verify its size
        diskFile.seekg(0, std::ios::end);
        size_t currentSize = diskFile.tellg();
        std::cout << "Existing disk file size: " << currentSize << " bytes." << std::endl;
        std::cout << "Expected disk file size: " << diskSize << " bytes." << std::endl;

        if (currentSize != diskSize) {
            diskFile.close();
            throw std::runtime_error("Existing disk file size does not match the expected size.");
        }
        std::cout << "Disk file loaded successfully." << std::endl;
    }

    // Close the file (it will be reopened for specific operations later)
    diskFile.close();
}

void writeBlock(size_t blockNumber, const std::string& data, const std::string& diskName) {
    if (data.size() > BLOCK_SIZE) {
        throw std::runtime_error("Data exceeds block size.");
    }

    // Validate block number
    validateBlockNumber(blockNumber);

    std::fstream diskFile(diskName, std::ios::in | std::ios::out | std::ios::binary);

    size_t offset = blockNumber * BLOCK_SIZE;

    // Validate disk file
    validateDiskFile(diskFile, offset);

    // Write data to the block
    diskFile.write(data.data(), data.size());
    if (data.size() < BLOCK_SIZE) {
        std::vector<char> padding(BLOCK_SIZE - data.size(), 0);
        diskFile.write(padding.data(), padding.size());
    }

    diskFile.close();
    std::cout << "Data written to block " << blockNumber << " successfully." << std::endl;
}


std::string readBlock(size_t blockNumber, const std::string& diskName) {
    // Validate block number
    validateBlockNumber(blockNumber);

    std::fstream diskFile(diskName, std::ios::in | std::ios::binary);

    size_t offset = blockNumber * BLOCK_SIZE;

    // Validate disk file
    validateDiskFile(diskFile, offset);

    std::vector<char> buffer(BLOCK_SIZE, 0); // Ensure full block is initialized
    diskFile.read(buffer.data(), BLOCK_SIZE);

    diskFile.close();
    // Ensure we return exactly BLOCK_SIZE bytes
    return std::string(buffer.begin(), buffer.end());
}

void deleteBlock(size_t blockIndex, const std::string& diskName) {
    // Open the disk file
    std::fstream diskFile(diskName, std::ios::in | std::ios::out | std::ios::binary);
    if (!diskFile.is_open()) {
        throw std::runtime_error("Failed to open disk file.");
    }

    // Check the disk file size
    diskFile.seekg(0, std::ios::end);
    size_t fileSize = diskFile.tellg();

    // Ensure the block index is valid
    if (blockIndex * BLOCK_SIZE >= fileSize) {
        throw std::out_of_range("Invalid block number.");
    }

    // Check for file corruption (simple integrity check)
    if (fileSize % BLOCK_SIZE != 0) {
        throw std::runtime_error("Corrupted disk file: file size is not a multiple of block size.");
    }

    // Create a buffer of zeros
    std::vector<char> zeroBuffer(BLOCK_SIZE, 0);

    // Seek to the block position
    diskFile.seekp(blockIndex * BLOCK_SIZE, std::ios::beg);

    // Overwrite the block with zeros
    diskFile.write(zeroBuffer.data(), BLOCK_SIZE);
    if (!diskFile.good()) {
        throw std::runtime_error("Failed to write to the disk file.");
    }

    // Close the file
    diskFile.close();
}



void validateBlockNumber(size_t blockNumber) {
    size_t maxBlocks = 1024 * 1024 / BLOCK_SIZE; // Disk size (1 MB) / block size (4 KB)
    if (blockNumber >= maxBlocks || blockNumber < 0) {
        throw std::out_of_range("Invalid block number.");
    }
}

void validateDiskFile(std::fstream& diskFile, size_t offset) {
    if (!diskFile) {
        throw std::runtime_error("Failed to open disk file");
    }

    diskFile.seekg(offset, std::ios::beg);
    if (!diskFile) {
        throw std::runtime_error("Failed to seek to block position.");
    }
}


#ifndef TESTING
int main() {
    try {
        std::string diskName = "virtual_disk.dat"; // Replace with your disk file name
        size_t blockIndex = 2; // Example block index to delete
        deleteBlock(blockIndex, diskName);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
#endif
