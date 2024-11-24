#include <gtest/gtest.h>
#include <fstream> // For std::ifstream
#include "disk_manager.h" // Include the header for BLOCK_SIZE and function declarations

TEST(DiskManagerTests, InitializeDisk) {
    const std::string diskName = "test_disk.dat";
    const size_t diskSize = 1024 * 1024; // 1 MB

    initializeDisk(diskName, diskSize);

    std::ifstream diskFile(diskName, std::ios::binary); // Ensure std::ifstream is included
    ASSERT_TRUE(diskFile.is_open());

    diskFile.seekg(0, std::ios::end);
    size_t fileSize = diskFile.tellg();
    ASSERT_EQ(fileSize, diskSize); // File size should match expected disk size
    diskFile.close();

    std::remove(diskName.c_str()); // Clean up test file
}

// Test: Write and Read Block
TEST(DiskManagerTests, WriteAndReadBlock) {
    const std::string diskName = "test_disk.dat";
    const size_t diskSize = 1024 * 1024; // 1 MB
    initializeDisk(diskName, diskSize);

    const std::string data = "Hello, Block!";
    writeBlock(0, data, diskName);

    std::string readData = readBlock(0, diskName);
    ASSERT_EQ(readData.substr(0, data.size()), data); // Ensure data matches
    ASSERT_EQ(readData.size(), BLOCK_SIZE);          // Block size should be consistent

    std::remove(diskName.c_str()); // Clean up test file
}

// Test: Invalid Block Number
TEST(DiskManagerTests, InvalidBlockNumber) {
    const std::string diskName = "test_disk.dat";
    const size_t diskSize = 1024 * 1024; // 1 MB
    initializeDisk(diskName, diskSize);

    ASSERT_THROW(writeBlock(256, "Out of range", diskName), std::runtime_error); // Invalid block
    ASSERT_THROW(readBlock(256, diskName), std::runtime_error); // Invalid block

    std::remove(diskName.c_str()); // Clean up test file
}
