#include <gtest/gtest.h>
#include "disk_manager.h"
#include "file_manager.h"
#include <fstream>
#include <sstream>
#include <cstdio> // For std::remove()

// Constants for DiskManager
const size_t MAX_BLOCKS_TEST = 256;
const size_t BLOCK_SIZE_TEST = 4096; // Assuming a block size of 4 KB
const std::string TEST_DISK_FILE = "test_disk_manager.dat";

// Helper function to initialize the disk file
void initializeDiskFile(const std::string& diskName) {
    std::ofstream disk(diskName, std::ios::binary | std::ios::trunc);
    std::string emptyData(MAX_BLOCKS_TEST * BLOCK_SIZE_TEST, '\0');
    disk.write(emptyData.c_str(), emptyData.size());
    disk.close();
}

// Test Fixture for DiskManager
class DiskManagerTests : public ::testing::Test {
protected:
    DiskManager* diskManager;

    void SetUp() override {
        // Create a new blank disk file before each test
        initializeDiskFile(TEST_DISK_FILE);
        diskManager = new DiskManager(TEST_DISK_FILE, MAX_BLOCKS_TEST);
    }

    void TearDown() override {
        // Clean up memory and delete the disk file after each test
        delete diskManager;
        std::remove(TEST_DISK_FILE.c_str());
    }
};

// **Test 1: Disk Initialization**
TEST_F(DiskManagerTests, InitializeDisk) {
    ASSERT_EQ(diskManager->getBitmap().getBitmap().size(), MAX_BLOCKS_TEST);
}

// **Test 2: Write and Read Block**
TEST_F(DiskManagerTests, WriteAndReadBlock) {
    std::string data = "Hello, World!";
    diskManager->writeBlock(0, data);

    // Read the block and validate
    std::string readData = diskManager->readBlock(0);
    ASSERT_EQ(readData, data) << "The data read from block 0 did not match the written data.";
}

// **Test 3: Delete Block**
TEST_F(DiskManagerTests, DeleteBlock) {
    const size_t blockIndex = 0;
    std::string data = "Test data for block";
    
    // Write data to the block
    diskManager->writeBlock(blockIndex, data);

    // Delete the block
    ASSERT_NO_THROW(diskManager->deleteBlock(blockIndex));

    // Verify the block is free by attempting to read it (assuming it throws an error)
    ASSERT_THROW(diskManager->readBlock(blockIndex), std::runtime_error);

    // Attempt to delete an already free block (should throw an error)
    ASSERT_THROW(diskManager->deleteBlock(blockIndex), std::runtime_error);
}

// **Test 4: Write to Multiple Blocks**
TEST_F(DiskManagerTests, WriteToMultipleBlocks) {
    std::string data1 = "Block 0 data";
    std::string data2 = "Block 1 data";
    
    diskManager->writeBlock(0, data1);
    diskManager->writeBlock(1, data2);

    // Validate both blocks
    ASSERT_EQ(diskManager->readBlock(0), data1) << "Block 0 data mismatch.";
    ASSERT_EQ(diskManager->readBlock(1), data2) << "Block 1 data mismatch.";
}

// **Test 5: Overwrite Block**
TEST_F(DiskManagerTests, OverwriteBlock) {
    std::string initialData = "Initial data";
    std::string newData = "New data after overwrite";

    diskManager->writeBlock(0, initialData);
    ASSERT_EQ(diskManager->readBlock(0), initialData) << "Initial data mismatch.";

    // Overwrite the block
    diskManager->writeBlock(0, newData);
    ASSERT_EQ(diskManager->readBlock(0), newData) << "Data after overwrite mismatch.";
}

// **Test 6: Invalid Block Index**
TEST_F(DiskManagerTests, InvalidBlockIndex) {
    // Trying to read a block out of range should throw an error
    ASSERT_THROW(diskManager->readBlock(MAX_BLOCKS_TEST), std::out_of_range);
    ASSERT_THROW(diskManager->writeBlock(MAX_BLOCKS_TEST, "Test"), std::out_of_range);
    ASSERT_THROW(diskManager->deleteBlock(MAX_BLOCKS_TEST), std::out_of_range);
}

// Test Fixture
class FileManagerTests : public ::testing::Test {
protected:
    DiskManager* diskManager;
    FileManager* fileManager;

    void SetUp() override {
        const std::string testDisk = "test_file_manager.dat";
        initializeDiskFile(testDisk);
        diskManager = new DiskManager(testDisk, MAX_BLOCKS_TEST);
        fileManager = new FileManager(*diskManager);
    }

    void TearDown() override {
        delete fileManager;
        delete diskManager;
        std::remove("test_file_manager.dat");
    }
};

// Test: Create a file successfully
TEST_F(FileManagerTests, CreateFileSuccess) {
    ASSERT_NO_THROW(fileManager->createFile("/file1.txt", 1024));
    const auto* metadata = fileManager->getMetadata("/file1.txt");
    ASSERT_NE(metadata, nullptr);
    EXPECT_EQ(metadata->name, "/file1.txt");
    EXPECT_EQ(metadata->type, FileType::File);
    EXPECT_EQ(metadata->size, 1024);
}

// Test: Create a directory successfully
TEST_F(FileManagerTests, CreateDirectorySuccess) {
    ASSERT_NO_THROW(fileManager->createDirectory("/mydir"));
    const auto* metadata = fileManager->getMetadata("/mydir");
    ASSERT_NE(metadata, nullptr);
    EXPECT_EQ(metadata->name, "/mydir");
    EXPECT_EQ(metadata->type, FileType::Directory);
}

// Test: Delete a file successfully
TEST_F(FileManagerTests, DeleteFileSuccess) {
    fileManager->createFile("/file1.txt", 1024);
    ASSERT_NO_THROW(fileManager->deleteFile("/file1.txt"));
    EXPECT_EQ(fileManager->getMetadata("/file1.txt"), nullptr);
}

// Test: Delete a directory successfully
TEST_F(FileManagerTests, DeleteDirectorySuccess) {
    fileManager->createDirectory("/mydir");
    ASSERT_NO_THROW(fileManager->deleteDirectory("/mydir"));
    EXPECT_EQ(fileManager->getMetadata("/mydir"), nullptr);
}

// Test: Create a file that already exists
TEST_F(FileManagerTests, CreateFileAlreadyExists) {
    fileManager->createFile("/file1.txt", 1024);
    EXPECT_THROW(fileManager->createFile("/file1.txt", 2048), std::runtime_error);
}

// Test: Create a directory that already exists
TEST_F(FileManagerTests, CreateDirectoryAlreadyExists) {
    fileManager->createDirectory("/mydir");
    EXPECT_THROW(fileManager->createDirectory("/mydir"), std::runtime_error);
}

// Test: Invalid file name
TEST_F(FileManagerTests, InvalidFileName) {
    EXPECT_THROW(fileManager->createFile("", 1024), std::invalid_argument);
    EXPECT_THROW(fileManager->createFile("/invalid@name.txt", 1024), std::invalid_argument);
}

// Test: Invalid directory name
TEST_F(FileManagerTests, InvalidDirectoryName) {
    EXPECT_THROW(fileManager->createDirectory(""), std::invalid_argument);
    EXPECT_THROW(fileManager->createDirectory("/invalid@dir"), std::invalid_argument);
}

// Test: Delete non-existent file
TEST_F(FileManagerTests, DeleteNonExistentFile) {
    EXPECT_THROW(fileManager->deleteFile("/nonexistent.txt"), std::runtime_error);
}

// Test: Delete non-existent directory
TEST_F(FileManagerTests, DeleteNonExistentDirectory) {
    EXPECT_THROW(fileManager->deleteDirectory("/nonexistent"), std::runtime_error);
}

// Test: List directory contents
TEST_F(FileManagerTests, ListDirectoryContents) {
    fileManager->createDirectory("/mydir");
    fileManager->createFile("/mydir/file1.txt", 1024);
    fileManager->createFile("/mydir/file2.txt", 2048);

    auto contents = fileManager->listDirectory("/mydir");
    EXPECT_EQ(contents.size(), 2);
    EXPECT_NE(std::find(contents.begin(), contents.end(), "/mydir/file1.txt"), contents.end());
    EXPECT_NE(std::find(contents.begin(), contents.end(), "/mydir/file2.txt"), contents.end());
}

// Test: Delete non-empty directory without recursive flag
TEST_F(FileManagerTests, DeleteNonEmptyDirectoryWithoutRecursive) {
    fileManager->createDirectory("/mydir");
    fileManager->createFile("/mydir/file1.txt", 1024);
    EXPECT_THROW(fileManager->deleteDirectory("/mydir", false), std::runtime_error);
}

// Test: Delete non-empty directory with recursive flag
TEST_F(FileManagerTests, DeleteNonEmptyDirectoryWithRecursive) {
    fileManager->createDirectory("/mydir");
    fileManager->createFile("/mydir/file1.txt", 1024);
    ASSERT_NO_THROW(fileManager->deleteDirectory("/mydir", true));
    EXPECT_EQ(fileManager->getMetadata("/mydir"), nullptr);
}

// Test: Normalize paths
TEST_F(FileManagerTests, NormalizePaths) {
    fileManager->createDirectory("/mydir");
    fileManager->createFile("/mydir/file1.txt", 1024);
    const auto* metadata = fileManager->getMetadata("/mydir/./file1.txt");
    ASSERT_NE(metadata, nullptr);
    EXPECT_EQ(metadata->name, "/mydir/file1.txt");
}

// Test: Guard against root deletion
TEST_F(FileManagerTests, RootDeletionGuard) {
    EXPECT_THROW(fileManager->deleteDirectory("/", true), std::runtime_error);
    EXPECT_THROW(fileManager->deleteDirectory("/root", true), std::runtime_error);
}

// Write and read data from a file
TEST_F(FileManagerTests, WriteAndReadFile) {
    // Create a file
    ASSERT_NO_THROW(fileManager->createFile("/file.txt", 0));

    // Write data
    std::string data1 = "Hello, ";
    std::string data2 = "World!";

    ASSERT_NO_THROW(fileManager->writeFile("/file.txt", data1, false));
    ASSERT_NO_THROW(fileManager->writeFile("/file.txt", data2, true));

    // Read data
    std::string result;
    ASSERT_NO_THROW(result = fileManager->readFile("/file.txt"));
    ASSERT_EQ(result, "Hello, World!");

    std::cout << "Final file content: " << result << std::endl;
}

// Overwrites a file
TEST_F(FileManagerTests, OverwriteFile) {
    // Create a file
    ASSERT_NO_THROW(fileManager->createFile("/file.txt", 0));

    // Write initial data
    ASSERT_NO_THROW(fileManager->writeFile("/file.txt", "Initial data", false));

    // Verify data
    std::string initialContent;
    ASSERT_NO_THROW(initialContent = fileManager->readFile("/file.txt"));
    ASSERT_EQ(initialContent, "Initial data");
    std::cout << "Initial file content: " << initialContent << std::endl;

    // Overwrite with new data
    ASSERT_NO_THROW(fileManager->writeFile("/file.txt", "New data", false));

    // Verify the new data
    std::string updatedContent;
    ASSERT_NO_THROW(updatedContent = fileManager->readFile("/file.txt"));
    ASSERT_EQ(updatedContent, "New data");
    std::cout << "Updated file content: " << updatedContent << std::endl;
}

TEST_F(FileManagerTests, OpenFile) {
    // Create a file
    fileManager->createFile("/example.txt", 0);
    
    // Write some content
    fileManager->writeFile("/example.txt", "This is a test file.", false);

    // Open and display the file
    ASSERT_NO_THROW(fileManager->openFile("/example.txt"));
}