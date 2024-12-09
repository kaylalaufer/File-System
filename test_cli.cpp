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

// Create file with valid path and size
TEST(CommandLineTest, CreateFileWithValidPathAndSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /validfile.txt 50\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("File created at /validfile.txt") != std::string::npos)
        << "Captured output: " << output;
}

// Create file with negative size
TEST(CommandLineTest, CreateFileWithNegativeSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /example.txt -100\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    std::cout << "Captured output: " << output << std::endl;

    EXPECT_TRUE(output.find("File size must be a positive number") != std::string::npos);
}

// Create file with size 0
TEST(CommandLineTest, CreateFileWithZeroSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /emptyfile.txt 0\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: File size must be a positive number.") != std::string::npos)
        << "Captured output: " << output;
}

// Create file with non integer size
TEST(CommandLineTest, CreateFileWithNonIntegerSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /example.txt abc\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Invalid size: 'abc'") != std::string::npos)
        << "Captured output: " << output;
}

// Create file with missing size
TEST(CommandLineTest, CreateFileWithMissingSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /example.txt\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: Invalid command. Usage: create_file [path] [size].") != std::string::npos)
        << "Captured output: " << output;
}

// Create file that already exists
TEST(CommandLineTest, CreateFileThatAlreadyExists) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /example.txt 100\ncreate_file /example.txt 100\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("File already exists") != std::string::npos)
        << "Captured output: " << output;
}

// Create file with max unsigned long size
TEST(CommandLineTest, CreateFileWithMaxUnsignedLongSize) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_file /largefile.txt 4294967295\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("File size is too large.") != std::string::npos)
        << "Captured output: " << output;
}

// Create File with Special Characters in Path
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

// Delete File that Exists
TEST(CommandLineTest, DeleteFileThatExists) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createFile("/delete_me.txt", 100);

    IOTestHelper ioHelper("delete_file /delete_me.txt\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("File deleted at /delete_me.txt") != std::string::npos)
        << "Captured output: " << output;
}

// Delete File that Does Not Exist
TEST(CommandLineTest, DeleteFileThatDoesNotExist) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("delete_file /not_a_file.txt\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: File does not exist.") != std::string::npos)
        << "Captured output: " << output;
}

// Delete File but Path is a Directory
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

// Create Directory
TEST(CommandLineTest, CreateDirectory) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_dir /new_folder\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Directory created at /new_folder") != std::string::npos)
        << "Captured output: " << output;
}

// Create Directory with Special Characters
TEST(CommandLineTest, CreateDirectoryWithSpecialCharacters) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_dir /!@#$%^&*()\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: Invalid directory name.") != std::string::npos)
        << "Captured output: " << output;
}

// Create directory with invalid path
TEST(CommandLineTest, CreateDirectoryWithInvalidPath) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("create_dir /!@#$%^&*()\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: Invalid directory name.") != std::string::npos)
        << "Captured output: " << output;
}

/* FILE READ AND WRITE TESTS */

// Write to File
TEST(CommandLineTest, WriteToFile) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createFile("/write_me.txt", 100);

    IOTestHelper ioHelper("write_file /write_me.txt HelloWorld false\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Data written to /write_me.txt") != std::string::npos)
        << "Captured output: " << output;
}

// Write to File that Doesn't Exist
TEST(CommandLineTest, WriteToNonExistentFile) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    IOTestHelper ioHelper("write_file /not_a_file.txt HelloWorld false\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Error: File does not exist.") != std::string::npos)
        << "Captured output: " << output;
}

// Read File
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

// List Directory Contents
TEST(CommandLineTest, ListDirectory) {
    DiskManager diskManager("test_disk.dat", 256);
    FileManager fileManager(diskManager);

    fileManager.createDirectory("/list_me");
    fileManager.createFile("/list_me/file1.txt", 100);
    fileManager.createFile("/list_me/file2.txt", 200);

    IOTestHelper ioHelper("list_dir /list_me\nexit\n");

    startCLI(fileManager);

    std::string output = ioHelper.getOutput();
    EXPECT_TRUE(output.find("Contents of /list_me:") != std::string::npos)
        << "Captured output: " << output;
}
