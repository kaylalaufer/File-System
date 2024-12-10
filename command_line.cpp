#include <iostream>
#include <sstream>
#include <regex>
#include "file_manager.h"
#include "command_line.h"


bool isSizeNumber(const std::string& str) {
    if (str.empty()) return false;
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

void startCLI(FileManager& fileManager) {
    std::cout << "Welcome to the FileManager CLI!" << std::endl;
    std::cout << "Type 'help' to see available commands." << std::endl;

    std::string commandLine;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, commandLine);

        std::istringstream iss(commandLine);
        std::string command;
        iss >> command;

        try {
            if (command == "help") {
                std::cout << "Available commands:" << std::endl;
                std::cout << "  create_file [path] [size]" << std::endl;
                std::cout << "  create_dir [path]" << std::endl;
                std::cout << "  delete_file [path]" << std::endl;
                std::cout << "  delete_dir [path] [recursive]" << std::endl;
                std::cout << "  write_file [path] [data] [append]" << std::endl;
                std::cout << "  read_file [path]" << std::endl;
                std::cout << "  list_dir [path]" << std::endl;
                std::cout << "  exit" << std::endl;
            } else if (command == "create_file") {
                std::string path, sizeStr;
                size_t size;
                iss >> path >> sizeStr;

                // Check if the path or size is empty
                if (path.empty() || sizeStr.empty()) {
                    std::cerr << "Error: Invalid command. Usage: create_file [path] [size]." << std::endl;
                    continue;
                } else if (sizeStr[0] == '-') { // Check if the size is valid
                    std::cerr << "Error: File size must be a positive number." << std::endl;
                    continue;
                } else if (!isSizeNumber(sizeStr)) {
                    std::cerr << "Invalid size: '" << sizeStr << "'. Please provide a valid number." << std::endl;
                    continue;
                } 

                // Validate the size
                try {
                    size = std::stoul(sizeStr); 
                } catch (const std::exception& e) {
                    std::cerr << "Invalid size: '" << sizeStr << "'. Please provide a valid number." << std::endl;
                } 

                if (size >= 1048576) {
                    std::cerr << "File size is too large. Please provide a number between 1 and 1048576." << std::endl;
                    continue;
                } else if (size <= 0) {
                    std::cerr << "Error: File size must be a positive number." << std::endl;
                    continue;
                }

                fileManager.createFile(path, size);
                std::cout << "File created at " << path << std::endl;

            } else if (command == "create_dir") {
                std::string path;
                iss >> path;
                fileManager.createDirectory(path);
                std::cout << "Directory created at " << path << std::endl;
            } else if (command == "delete_file") {
                std::string path;
                iss >> path;
                fileManager.deleteFile(path);
                std::cout << "File deleted at " << path << std::endl;
            } else if (command == "delete_dir") {
                std::string path;
                std::string recursiveStr;
                bool recursive = false;
                iss >> path >> recursiveStr;
                if (recursiveStr == "true") recursive = true;
                fileManager.deleteDirectory(path, recursive);
                std::cout << "Directory deleted at " << path << std::endl;
            } else if (command == "write_file") {
                std::string path, data, appendStr;
                bool append = true;
                iss >> path;

                // 🔥 Read the entire remaining line as data
                std::getline(iss, data);

                // 🔥 Remove leading and trailing whitespace
                data.erase(0, data.find_first_not_of(" \t"));
                data.erase(data.find_last_not_of(" \t") + 1);

                // 🔥 Extract the quoted string if present
                std::smatch match;
                std::regex quotedRegex(R"delim("([^"]*)")delim");
                if (std::regex_search(data, match, quotedRegex)) {
                    data = match[1].str(); // Extract the content inside the quotes
                }

                // 🔥 Extract possible append flag

                size_t spacePos = data.find_last_of(' '); 
                if (spacePos != std::string::npos) {
                    appendStr = data.substr(spacePos + 1); // Extract last part
                    if (appendStr == "true" || appendStr == "false") {
                        data = data.substr(0, spacePos);  // Extract actual data
                        append = (appendStr == "true");   // Set append flag
                    }
                }

                // 🔥 Remove leading and trailing whitespace again to clean up
                data.erase(0, data.find_first_not_of(" \t"));
                data.erase(data.find_last_not_of(" \t") + 1);

                std::cout << "Data to write: " << data << std::endl;
                fileManager.writeFile(path, data, append);
                std::cout << "Data written to " << path << std::endl;
            } else if (command == "read_file") {
                std::string path;
                iss >> path;
                std::string content = fileManager.readFile(path);
                std::cout << "Contents of " << path << ":\n" << content << std::endl;
            } else if (command == "list_dir") {
                std::string path;
                iss >> path;
                auto contents = fileManager.listDirectory(path);
                std::cout << "Contents of " << path << ":" << std::endl;
                for (const auto& entry : contents) {
                    std::cout << "  " << entry << std::endl;
                }
            } else if (command == "exit") {
                std::cout << "Exiting CLI. Goodbye!" << std::endl;
                break;
            } else {
                std::cout << "Unknown command. Type 'help' for a list of commands." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
#ifndef TEST_BUILD
int main() {
    const std::string diskName = "cli_disk.dat";
    const std::string fileSystemDataFile = "filesystem.dat";

    DiskManager diskManager(diskName, 256);
    FileManager fileManager(diskManager);

    

    // 🔥 Load existing file system metadata
    std::ifstream fsFile(fileSystemDataFile, std::ios::binary);
    if (fsFile) {
        std::cout << "in load file" << std::endl;
        fileManager.load(fsFile);
        fsFile.close();
    }

    

    startCLI(fileManager);

    // 🔥 Save disk contents before exit
    /*std::ofstream outFile(diskName, std::ios::binary);
    diskManager.save(outFile);
    outFile.close();*/

    // 🔥 Save file system metadata before exit
    std::ofstream fsOut(fileSystemDataFile, std::ios::binary);
    fileManager.save(fsOut);
    fsOut.close();

    return 0;
}
#endif
/*int main() {
    const std::string diskName = "cli_disk.dat";
    const std::string fileSystemDataFile = "filesystem.dat";
    DiskManager diskManager(diskName, 256);
    FileManager fileManager(diskManager);

    // Load existing file system metadata
    std::ifstream inFile(fileSystemDataFile, std::ios::binary);
    if (inFile) {
        fileManager.load(inFile);
        inFile.close();
    }

    startCLI(fileManager);

    // Save file system metadata on exit
    std::ofstream outFile(fileSystemDataFile, std::ios::binary);
    fileManager.save(outFile);
    outFile.close();

    return 0;
    
    
    const std::string diskName = "cli_disk.dat";
    const size_t numBlocks = 256;

    // Initialize DiskManager and FileManager
    DiskManager diskManager(diskName, numBlocks);
    FileManager fileManager(diskManager);

    loadFileSystem(fileManager);
    startCLI(fileManager);
    saveFileSystem(fileManager);

    return 0;
}*/