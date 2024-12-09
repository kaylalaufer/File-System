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


/* CREATE DIRECTORY TESTS */


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

