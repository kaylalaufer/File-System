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

    ASSERT_THROW(writeBlock(256, "Out of range", diskName), std::out_of_range); // Invalid block
    ASSERT_THROW(readBlock(256, diskName), std::out_of_range); // Invalid block

    ASSERT_THROW(writeBlock(-1, "Negative Index", diskName), std::out_of_range); // Invalid block
    ASSERT_THROW(readBlock(-1, diskName), std::out_of_range); // Invalid block

    std::remove(diskName.c_str()); // Clean up test file
}

// Test: Delete a block from a valid disk file
TEST(DiskManagerTests, DeleteBlockValid) {
    const std::string diskName = "test_disk.dat";
    const size_t diskSize = 1024 * 1024; // 1 MB

    // Initialize the disk
    initializeDisk(diskName, diskSize);

    // Write some data to block 0
    std::string data(BLOCK_SIZE, 'X');
    writeBlock(0, diskName, data);

    // Delete the block
    deleteBlock(0, diskName);

    // Read the block to verify it's zeroed out
    std::string readData = readBlock(0, diskName);
    std::string zeroBlock(BLOCK_SIZE, '\0');
    ASSERT_EQ(readData, zeroBlock);

    // Clean up
    std::remove(diskName.c_str());
}


// Test: Delete a block from an invalid block index
TEST(DiskManagerTests, DeleteBlockInvalidIndex) {
    const std::string diskName = "test_disk.dat";
    const size_t diskSize = 1024 * 1024; // 1 MB

    // Initialize the disk
    initializeDisk(diskName, diskSize);

    const size_t invalidIndex = 300; // Out of range index
    ASSERT_THROW(deleteBlock(invalidIndex, diskName), std::out_of_range);

    // Clean up
    std::remove(diskName.c_str());
}

// Test: Delete a block when the disk file does not exist
TEST(DiskManagerTests, DeleteBlockNonExistentFile) {
    const std::string nonExistentDisk = "non_existent_disk.dat";
    const size_t blockIndex = 0;

    ASSERT_THROW(deleteBlock(blockIndex, nonExistentDisk), std::runtime_error);
}

// Test: Delete a block when the disk file is corrupted
TEST(DiskManagerTests, DeleteBlockCorruptedFile) {
    const std::string corruptedDisk = "corrupted_disk.dat";

    // Create a corrupted disk file
    std::ofstream diskFile(corruptedDisk, std::ios::binary);
    diskFile.seekp(BLOCK_SIZE / 2, std::ios::beg); // Write half a block
    diskFile.write("\0", 1);
    diskFile.close();

    const size_t blockIndex = 0;
    ASSERT_THROW(deleteBlock(blockIndex, corruptedDisk), std::runtime_error);

    // Clean up
    std::remove(corruptedDisk.c_str());
}


// Test: Delete a block with negative index
TEST(DiskManagerTests, DeleteBlockNegativeIndex) {
    const std::string diskName = "test_disk.dat";
    const size_t diskSize = 1024 * 1024; // 1 MB

    // Initialize the disk
    initializeDisk(diskName, diskSize);

    const int negativeIndex = -1; // Invalid negative index
    ASSERT_THROW(deleteBlock(negativeIndex, diskName), std::invalid_argument);

    // Clean up
    std::remove(diskName.c_str());
}