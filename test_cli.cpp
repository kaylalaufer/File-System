#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <string>
#include "file_manager.h"
#include "disk_manager.h"
#include "command_line.h"

class IOTestHelper {
public:
    IOTestHelper(const std::string& input) 
        : inputBuffer(input), 
          originalCin(std::cin.rdbuf()), 
          originalCout(std::cout.rdbuf()), 
          originalCerr(std::cerr.rdbuf()) 
    {
        std::cin.rdbuf(inputBuffer.rdbuf());
        std::cout.rdbuf(outputBuffer.rdbuf());
        std::cerr.rdbuf(outputBuffer.rdbuf());
    }

    ~IOTestHelper() {
        std::cin.rdbuf(originalCin);
        std::cout.rdbuf(originalCout);
        std::cerr.rdbuf(originalCerr);
    }

    std::string getOutput() const {
        return outputBuffer.str();
    }

private:
    std::istringstream inputBuffer;
    std::ostringstream outputBuffer;
    std::streambuf* originalCin;
    std::streambuf* originalCout;
    std::streambuf* originalCerr;
};

// Test Helper Function
TEST(IOTestHelperTest, CapturesCoutAndCerr) {
    IOTestHelper ioHelper("");
    std::cout << "This is std::cout" << std::endl;
    std::cerr << "This is std::cerr" << std::endl;

    std::string output = ioHelper.getOutput();

    std::cout << "Captured output: " << output << std::endl;

    EXPECT_NE(output.find("This is std::cout"), std::string::npos);
    EXPECT_NE(output.find("This is std::cerr"), std::string::npos);
}

class CommandLineTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::remove("test_disk.dat"); // Clear disk file
        std::remove("filesystem.dat"); // Clear file system metadata
    }

    void TearDown() override {
        std::remove("test_disk.dat");
        std::remove("filesystem.dat");
    }
};

/* CREATE FILE TESTS */

// Test: Create file with valid path and size
TEST(CommandLineTest, CreateFileWithValidPathAndSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /validfile.txt 50\nexit\n");
    startCLI(fileManager);

    const FileEntry* fileEntry = fileManager.getMetadata("/validfile.txt");
    EXPECT_TRUE(fileEntry != nullptr) << "File /validfile.txt should exist.";
    EXPECT_TRUE(fileEntry->type == FileType::File) << "/validfile.txt should be a file.";
    EXPECT_EQ(fileEntry->size, 50) << "/validfile.txt should have a size of 50.";
}

// Test: Create file with negative size
TEST(CommandLineTest, CreateFileWithNegativeSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /example.txt -100\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    std::cout << "Captured output: " << output << std::endl;

    EXPECT_TRUE(output.find("File size must be a positive number") != std::string::npos);
}

// Test: Create file with size 0
TEST(CommandLineTest, CreateFileWithZeroSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /emptyfile.txt 0\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: File size must be a positive number.") != std::string::npos)
        << "Captured output: " << output;
}

// Test: Create file with non integer size
TEST(CommandLineTest, CreateFileWithNonIntegerSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /example.txt abc\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Invalid size: 'abc'") != std::string::npos)
        << "Captured output: " << output;
}

// Test: Create file with missing size
TEST(CommandLineTest, CreateFileWithMissingSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /example.txt\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Default file size is 100.") != std::string::npos)
        << "Captured output: " << output;
}

// Test: Create file that already exists
TEST(CommandLineTest, CreateFileThatAlreadyExists) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /example.txt 100\ncreate_file /example.txt 100\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("File already exists") != std::string::npos)
        << "Captured output: " << output;
}

// Test: Create file with max unsigned long size
TEST(CommandLineTest, CreateFileWithMaxUnsignedLongSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /largefile.txt 4294967295\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("File size is too large.") != std::string::npos)
        << "Captured output: " << output;
}

// Test: Create File with Special Characters in Path
TEST(CommandLineTest, CreateFileWithSpecialCharactersInPath) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /!@#$%^&*() 100\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: Invalid file name.") != std::string::npos)
        << "Captured output: " << output;
}

/* DELETE FILE TESTS */

// Test: Delete File that Exists
TEST(CommandLineTest, DeleteFileThatExists) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createFile("/delete_me.txt", 100);
    IOTestHelper ioHelper("delete_file /delete_me.txt\nexit\n");
    startCLI(fileManager);

    const FileEntry* fileEntry = fileManager.getMetadata("/delete_me.txt");
    EXPECT_TRUE(fileEntry == nullptr) << "File /delete_me.txt should no longer exist.";
}


// Test: Delete File that Does Not Exist
TEST(CommandLineTest, DeleteFileThatDoesNotExist) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("delete_file /not_a_file.txt\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: File does not exist.") != std::string::npos)
        << "Captured output: " << output;
}

// Test: Delete File but Path is a Directory
TEST(CommandLineTest, DeleteFileThatIsADirectory) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createDirectory("/my_folder");

    IOTestHelper ioHelper("delete_file /my_folder\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Path is not a file.") != std::string::npos)
        << "Captured output: " << output;
}

/* CREATE DIRECTORY TESTS */

// Test: Create Directory
TEST(CommandLineTest, CreateDirectory) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_dir /new_folder\nexit\n");
    startCLI(fileManager);

    const FileEntry* dirEntry = fileManager.getMetadata("/new_folder");
    EXPECT_TRUE(dirEntry != nullptr) << "Directory /new_folder should exist.";
    EXPECT_TRUE(dirEntry->type == FileType::Directory) << "/new_folder should be a directory.";
}


// Test: Create Directory with Special Characters
TEST(CommandLineTest, CreateDirectoryWithSpecialCharacters) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_dir /!@#$%^&*()\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: Invalid directory name.") != std::string::npos)
        << "Captured output: " << output;
}

// Test: Create directory with invalid path
TEST(CommandLineTest, CreateDirectoryWithInvalidPath) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_dir /!@#$%^&*()\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: Invalid directory name.") != std::string::npos)
        << "Captured output: " << output;
}

/* DELETE DIRECTORY TESTS */

// Test: Delete Empty Directory**
TEST(CommandLineTest, DeleteEmptyDirectory) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createDirectory("/empty_dir");

    // Ensure the directory exists
    const FileEntry* dirEntry = fileManager.getMetadata("/empty_dir");
    ASSERT_TRUE(dirEntry != nullptr) << "Directory /empty_dir should exist.";

    // Delete the empty directory
    fileManager.deleteDirectory("/empty_dir", false);

    // Verify the directory is deleted
    dirEntry = fileManager.getMetadata("/empty_dir");
    EXPECT_TRUE(dirEntry == nullptr) << "Directory /empty_dir should no longer exist.";
}

// Test: Delete Non-Empty Directory Without Recursive**
TEST(CommandLineTest, DeleteNonEmptyDirectoryWithoutRecursive) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createDirectory("/parent_dir");
    fileManager.createFile("/parent_dir/file1.txt", 50);

    // Ensure the directory exists
    const FileEntry* dirEntry = fileManager.getMetadata("/parent_dir");
    ASSERT_TRUE(dirEntry != nullptr) << "Directory /parent_dir should exist.";

    // Attempt to delete the non-empty directory without recursive, expect an error
    try {
        fileManager.deleteDirectory("/parent_dir", false);
        FAIL() << "Expected runtime_error due to non-empty directory";
    } catch (const std::runtime_error& err) {
        EXPECT_STREQ(err.what(), "Directory is not empty.");
    } catch (...) {
        FAIL() << "Expected runtime_error due to non-empty directory";
    }

    // Verify the directory still exists
    dirEntry = fileManager.getMetadata("/parent_dir");
    EXPECT_TRUE(dirEntry != nullptr) << "Directory /parent_dir should still exist.";
}

// Test: Delete Non-Empty Directory With Recursive**
TEST(CommandLineTest, DeleteNonEmptyDirectoryWithRecursive) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    // Create directory structure
    fileManager.createDirectory("/parent_dir");
    fileManager.createFile("/parent_dir/file1.txt", 50);
    fileManager.createFile("/parent_dir/file2.txt", 100);

    // Verify the directory structure exists
    ASSERT_TRUE(fileManager.getMetadata("/parent_dir") != nullptr) << "Directory /parent_dir should exist.";
    ASSERT_TRUE(fileManager.getMetadata("/parent_dir/file1.txt") != nullptr) << "File /parent_dir/file1.txt should exist.";
    ASSERT_TRUE(fileManager.getMetadata("/parent_dir/file2.txt") != nullptr) << "File /parent_dir/file2.txt should exist.";

    // Delete the non-empty directory recursively
    fileManager.deleteDirectory("/parent_dir", true);

    // Check if directory and contents are deleted using list_dir
    IOTestHelper ioHelper("list /parent_dir\nexit\n");
    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Directory does not exist.") != std::string::npos)
        << "Expected 'Directory does not exist.' but got: " << output;

    // Check that the files no longer exist using list_dir on root to ensure no remnants exist
    IOTestHelper ioHelperRoot("list /\nexit\n");
    startCLI(fileManager);

    std::string rootOutput = ioHelperRoot.getOutput();
    EXPECT_TRUE(rootOutput.find("file1.txt") == std::string::npos) 
        << "File /parent_dir/file1.txt should no longer exist in root directory: " << rootOutput;
    EXPECT_TRUE(rootOutput.find("file2.txt") == std::string::npos) 
        << "File /parent_dir/file2.txt should no longer exist in root directory: " << rootOutput;
}


// Test: Delete Non-Existent Directory**
TEST(CommandLineTest, DeleteNonExistentDirectory) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    // Attempt to delete a non-existent directory, expect an error
    try {
        fileManager.deleteDirectory("/non_existent_dir", false);
        FAIL() << "Expected runtime_error due to non-existent directory";
    } catch (const std::runtime_error& err) {
        EXPECT_STREQ(err.what(), "Directory does not exist.");
    } catch (...) {
        FAIL() << "Expected runtime_error due to non-existent directory";
    }
}

/* FILE READ AND WRITE TESTS */

// Test: Write to File
TEST(CommandLineTest, WriteToFile) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createFile("/write_me.txt", 100);
    IOTestHelper ioHelper("write_file /write_me.txt HelloWorld false\nexit\n");

    startCLI(fileManager);

    const FileEntry* fileEntry = fileManager.getMetadata("/write_me.txt");
    ASSERT_TRUE(fileEntry != nullptr) << "File /write_me.txt should exist.";
    std::string fileData = fileManager.readFile("/write_me.txt");
    EXPECT_EQ(fileData, "HelloWorld") << "Data in /write_me.txt should match.";
}

// Test: Write to File that Doesn't Exist
TEST(CommandLineTest, WriteToNonExistentFile) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("write_file /not_a_file.txt HelloWorld false\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: File does not exist.") != std::string::npos)
        << "Captured output: " << output;
}

// Test: Read File
TEST(CommandLineTest, ReadFile) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createFile("/read_me.txt", 100);
    fileManager.writeFile("/read_me.txt", "Hello, World!", false);

    IOTestHelper ioHelper("read_file /read_me.txt\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Hello, World!") != std::string::npos)
        << "Captured output: " << output;
}

// Test: List Directory Contents
TEST(CommandLineTest, ListDirectory) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createDirectory("/list_me");
    fileManager.createFile("/list_me/file1.txt", 100);
    fileManager.createFile("/list_me/file2.txt", 200);

    IOTestHelper ioHelper("list /list_me\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Contents of /list_me:") != std::string::npos)
        << "Captured output: " << output;
}

/* MOVE TESTS */

// Test: Move file to a new directory
TEST(CommandLineTest, MoveFileToNewDirectory) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createDirectory("/folder");
    fileManager.createFile("/folder/file1.txt", 100);
    fileManager.createDirectory("/new_folder");

    IOTestHelper ioHelper("move_file /folder/file1.txt /new_folder/file2.txt\nexit\n");
    startCLI(fileManager);

    // Check that the source file is gone
    const FileEntry* oldFileEntry = fileManager.getMetadata("/folder/file1.txt");
    ASSERT_TRUE(oldFileEntry == nullptr) << "File /folder/file1.txt should no longer exist.";

    // Check that the file is at the new location
    const FileEntry* newFileEntry = fileManager.getMetadata("/new_folder/file2.txt");
    ASSERT_TRUE(newFileEntry != nullptr) << "File /new_folder/file2.txt should exist.";
}

// Test: Move file to a non-existent directory (should fail)
TEST(CommandLineTest, MoveFileToNonExistentDirectory) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createFile("/file1.txt", 100);

    IOTestHelper ioHelper("move_file /file1.txt /non_existent_folder/file2.txt\nexit\n");
    startCLI(fileManager);

    const FileEntry* originalFileEntry = fileManager.getMetadata("/file1.txt");
    ASSERT_TRUE(originalFileEntry != nullptr) << "File /file1.txt should still exist.";
}

// Test: Move empty file
TEST(CommandLineTest, MoveEmptyFile) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createFile("/empty_file", 100); // Empty file, no data written

    fileManager.createDirectory("/empty_folder");

    IOTestHelper ioHelper("move_file /empty_file /empty_folder/empty_file\nexit\n");
    startCLI(fileManager);

    // Check that the new file exists
    const FileEntry* newFileEntry = fileManager.getMetadata("/empty_folder/empty_file");
    ASSERT_TRUE(newFileEntry != nullptr) << "File /empty_folder/empty_file should exist.";
    EXPECT_TRUE(newFileEntry->type == FileType::File) << "/empty_folder/empty_file should be a file.";
}

// Test: Move and check that content is retained
TEST(CommandLineTest, MoveFileAndRetainContent) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createFile("/file3", 100);
    fileManager.writeFile("/file3", "Hello, World!", false);

    fileManager.createDirectory("/my_folder");

    IOTestHelper ioHelper("move_file /file3 /my_folder/file3\nexit\n");
    startCLI(fileManager);

    // Check that the new file exists
    const FileEntry* newFileEntry = fileManager.getMetadata("/my_folder/file3");
    ASSERT_TRUE(newFileEntry != nullptr) << " File /my_folder/file3 should exist.";
    EXPECT_TRUE(newFileEntry->type == FileType::File) << "/my_folder/file3 should be a file.";

    // Check that the file's contents are retained
    std::string fileData = fileManager.readFile("/my_folder/file3");
    EXPECT_EQ(fileData, "Hello, World!") << "Contents of /my_folder/file3 should be 'Hello, World!'.";
}

/* GENERAL TESTS */

// Help Command
TEST(CommandLineTest, HelpCommand) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("help\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Available commands:") != std::string::npos)
        << "Captured output: " << output;
}