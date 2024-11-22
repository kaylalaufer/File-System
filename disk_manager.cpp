#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstring>

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

    std::fstream diskFile(diskName, std::ios::in | std::ios::out | std::ios::binary);
    if (!diskFile) {
        throw std::runtime_error("Failed to open disk file");
    }

    size_t offset = blockNumber * BLOCK_SIZE;
    diskFile.seekg(offset, std::ios::beg);
    if (!diskFile) {
        throw std::runtime_error("Failed to seek to block position.");
    }

    //Write data to the block
    diskFile.write(data.data(), data.size());
    if (data.size() < BLOCK_SIZE) {
        // Fill the rest of the block with zeros
        std::vector<char>padding(BLOCK_SIZE - data.size(), 0);
        diskFile.write(padding.data(), padding.size());
    }

    diskFile.close();
    std::cout << "Data written to block " << blockNumber << " successfully." << std::endl;
}

int main() {
    try {
        initializeDisk("virtual_disk.dat", 1024 * 1024); // 1 MB disk
        std::cout << "Disk initialization complete." << std::endl;

        // Write some data to block 0
        writeBlock(0, "Hello, Block 0!", "virtual_disk.dat");

        writeBlock(1, "Hi Block 1", "virtual_disk.dat");

        std::cout << "Write operations complete." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
