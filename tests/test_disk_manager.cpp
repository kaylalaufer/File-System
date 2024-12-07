#include <gtest/gtest.h>
#include "disk_manager.h"
#include <fstream>
#include <sstream>

// Constants
const std::string TEST_DISK = "test_disk.dat";

// Helper function to initialize the disk
void initializeDiskFile(const std::string& diskName) {
    std::ofstream disk(diskName, std::ios::binary | std::ios::trunc);
    std::string emptyData(MAX_BLOCKS * BLOCK_SIZE, '\0');
    disk.write(emptyData.c_str(), emptyData.size());
    disk.close();
}

// Helper function to check if a block is free
bool isBlockFree(const DiskManager& diskManager, size_t blockIndex) {
    return diskManager.getBitmap().isFree(blockIndex);
}

// Test case: Initialize Disk
TEST(DiskManagerTests, InitializeDisk) {
    const std::string diskName = "test_initialize.dat";
    initializeDiskFile(diskName);

    DiskManager diskManager(diskName, MAX_BLOCKS);
    ASSERT_EQ(diskManager.getBitmap().getBitmap().size(), MAX_BLOCKS);
}

// Test case: Write and Read Block
TEST(DiskManagerTests, WriteAndReadBlock) {
    const std::string diskName = "test_write_read.dat";
    initializeDiskFile(diskName);

    DiskManager diskManager(diskName, MAX_BLOCKS);
    std::string data = "Hello, World!";
    diskManager.writeBlock(0, data);

    // Read the block and validate
    ASSERT_EQ(diskManager.readBlock(0), data);
}

// Test case: Write Data Exceeding Block Size
TEST(DiskManagerTests, WriteExceedingBlockSize) {
    const std::string diskName = "test_exceed_size.dat";
    initializeDiskFile(diskName);

    DiskManager diskManager(diskName, MAX_BLOCKS);
    std::string largeData(BLOCK_SIZE + 1, 'A'); // Data larger than block size

    ASSERT_THROW(diskManager.writeBlock(0, largeData), std::runtime_error);
}

// Test case: Delete Block
TEST(DiskManagerTests, DeleteBlock) {
    const std::string diskName = "test_delete.dat";
    initializeDiskFile(diskName);

    DiskManager diskManager(diskName, MAX_BLOCKS);
    const size_t blockIndex = 0;

    // Write data to the block
    std::string data = "Test data for block";
    diskManager.writeBlock(blockIndex, data);

    // Delete the block (should succeed)
    ASSERT_NO_THROW(diskManager.deleteBlock(blockIndex));

    // Verify the block is free
    ASSERT_TRUE(isBlockFree(diskManager, blockIndex));
    ASSERT_THROW(diskManager.readBlock(blockIndex), std::runtime_error);

    // Attempt to delete an already free block (should throw)
    ASSERT_THROW(diskManager.deleteBlock(blockIndex), std::runtime_error);
}

// Test case: Out-of-Bounds Access
TEST(DiskManagerTests, OutOfBoundsAccess) {
    const std::string diskName = "test_out_of_bounds.dat";
    initializeDiskFile(diskName);

    DiskManager diskManager(diskName, MAX_BLOCKS);

    ASSERT_THROW(diskManager.writeBlock(MAX_BLOCKS, "Test"), std::out_of_range);
    ASSERT_THROW(diskManager.readBlock(MAX_BLOCKS), std::out_of_range);
    ASSERT_THROW(diskManager.deleteBlock(MAX_BLOCKS), std::out_of_range);
}