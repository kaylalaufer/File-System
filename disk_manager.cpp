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
    size_t maxBlocks = 1024 * 1024 / BLOCK_SIZE; // Disk size (1 MB) / block size (4 KB)
    if (blockNumber >= maxBlocks) {
        throw std::runtime_error("Invalid block number.");
    }

    std::fstream diskFile(diskName, std::ios::in | std::ios::out | std::ios::binary);
    if (!diskFile) {
        throw std::runtime_error("Failed to open disk file.");
    }

    size_t offset = blockNumber * BLOCK_SIZE;
    diskFile.seekp(offset, std::ios::beg);
    if (!diskFile) {
        throw std::runtime_error("Failed to seek to block position.");
    }

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
    std::fstream diskFile(diskName, std::ios::in | std::ios::binary);
    if (!diskFile) {
        throw std::runtime_error("Failed to open disk file");
    }

    // Validate block number
    size_t maxBlocks = 1024 * 1024 / BLOCK_SIZE; // Disk size (1 MB) / block size (4 KB)
    if (blockNumber >= maxBlocks) {
        throw std::runtime_error("Invalid block number.");
    }

    size_t offset = blockNumber * BLOCK_SIZE;

    diskFile.seekg(offset, std::ios::beg);
    if (!diskFile) {
        throw std::runtime_error("Failed to seek to block position.");
    }

    std::vector<char> buffer(BLOCK_SIZE, 0); // Ensure full block is initialized
    diskFile.read(buffer.data(), BLOCK_SIZE);

    diskFile.close();
    // Ensure we return exactly BLOCK_SIZE bytes
    return std::string(buffer.begin(), buffer.end());
}



void testWriteReadEdgeCases() {
    const std::string diskName = "test_disk.dat";
    const size_t diskSize = 1024 * 1024; // 1 MB
    const size_t blockSize = BLOCK_SIZE;

    try {
        initializeDisk(diskName, diskSize);

        // Edge Case 1: Data larger than block size
        try {
            std::string largeData(blockSize + 100, 'A'); // 100 bytes too large
            writeBlock(0, largeData, diskName);
        } catch (const std::exception& e) {
            std::cout << "Caught expected error: " << e.what() << std::endl;
        }

        // Edge Case 2: Empty data
        writeBlock(1, "", diskName);
        std::string emptyData = readBlock(1, diskName);
        std::cout << "Empty block data (first 16 bytes): ";
        for (size_t i = 0; i < 16; ++i) {
            std::cout << (int)emptyData[i] << " ";
        }
        std::cout << std::endl;

        // Edge Case 3: Invalid block number
        try {
            writeBlock(diskSize / blockSize, "Out of range", diskName); // Invalid block
        } catch (const std::exception& e) {
            std::cout << "Caught expected error: " << e.what() << std::endl;
        }

        // Edge Case 4: Missing disk file
        try {
            readBlock(0, "missing_disk.dat"); // Non-existent file
        } catch (const std::exception& e) {
            std::cout << "Caught expected error: " << e.what() << std::endl;
        }

        // Edge Case 5: Reading unwritten block
        std::string unwrittenData = readBlock(2, diskName);
        std::cout << "Unwritten block data (first 16 bytes): ";
        for (size_t i = 0; i < 16; ++i) {
            std::cout << (int)unwrittenData[i] << " ";
        }
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error during edge case testing: " << e.what() << std::endl;
    }
}

#ifndef TESTING
int main() {
    try {
        initializeDisk("virtual_disk.dat", 1024 * 1024); // 1 MB disk
        std::cout << "Disk initialization complete." << std::endl;

        // Write some data to block 0
        writeBlock(0, "Hello, Block 0!", "virtual_disk.dat");

        std::string data = readBlock(0, "virtual_disk.dat");
        std::cout << "Read from block 0: " << data << std::endl;

        testWriteReadEdgeCases();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
#endif
