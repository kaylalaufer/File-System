#include <gtest/gtest.h>
#include "disk_manager.h"
#include "file_manager.h"
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
    ASSERT_THROW(diskManager.readBlock(blockIndex), std::runtime_error);

    // Attempt to delete an already free block (should throw)
    ASSERT_THROW(diskManager.deleteBlock(blockIndex), std::runtime_error);
}

// Test Fixture
class FileManagerTests : public ::testing::Test {
protected:
    DiskManager* diskManager;
    FileManager* fileManager;

    void SetUp() override {
        //const std::string testDisk = "test_disk_" + std::string(::testing::UnitTest::GetInstance()->current_test_info()->test_case_name()) + ".dat";
        const std::string testDisk = "test_file_manager.dat";
        initializeDiskFile(testDisk);
        diskManager = new DiskManager(testDisk, MAX_BLOCKS);
        fileManager = new FileManager(*diskManager);
    }

    void TearDown() override {
        delete fileManager;
        delete diskManager;
        std::remove(TEST_DISK.c_str());
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
    initializeDiskFile("test_wr_disk.dat"); // Ensure the disk is initialized

    DiskManager diskManager("test_wr_disk.dat", MAX_BLOCKS); // Initialize DiskManager
    FileManager fileManager(diskManager); // Pass DiskManager to FileManager

    // Create a file
    ASSERT_NO_THROW(fileManager.createFile("/file.txt", 0));

    // Write data
    std::string data1 = "Hello, ";
    std::string data2 = "World!";

    ASSERT_NO_THROW(fileManager.writeFile("/file.txt", data1, false));
    ASSERT_NO_THROW(fileManager.writeFile("/file.txt", data2, true));

    // Read data
    std::string result;
    ASSERT_NO_THROW(result = fileManager.readFile("/file.txt"));
    ASSERT_EQ(result, "Hello, World!");

    std::cout << "Final file content: " << result << std::endl;
}

// Overwrites a file
TEST_F(FileManagerTests, OverwriteFile) {
    initializeDiskFile("test_ow_disk.dat"); // Ensure the disk is initialized

    DiskManager diskManager("test_ow_disk.dat", MAX_BLOCKS); // Initialize DiskManager
    FileManager fileManager(diskManager); // Pass DiskManager to FileManager

    // Create a file
    ASSERT_NO_THROW(fileManager.createFile("/file.txt", 0));

    // Write initial data
    ASSERT_NO_THROW(fileManager.writeFile("/file.txt", "Initial data", false));

    // Verify data
    std::string initialContent;
    ASSERT_NO_THROW(initialContent = fileManager.readFile("/file.txt"));
    ASSERT_EQ(initialContent, "Initial data");
    std::cout << "Initial file content: " << initialContent << std::endl;

    // Overwrite with new data
    ASSERT_NO_THROW(fileManager.writeFile("/file.txt", "New data", false));

    // Verify the new data
    std::string updatedContent;
    ASSERT_NO_THROW(updatedContent = fileManager.readFile("/file.txt"));
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