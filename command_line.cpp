#include <iostream>
#include <sstream>
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
                } else if (!isNumber(sizeStr)) {
                    std::cerr << "Invalid size: '" << sizeStr << "'. Please provide a valid number." << std::endl;
                    continue;
                }

                // Validate the size
                try {
                    size = std::stoul(sizeStr); 
                } catch (const std::exception& e) {
                    std::cerr << "Invalid size: '" << sizeStr << "'. Please provide a valid number." << std::endl;
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
                bool append = false;
                iss >> path >> data >> appendStr;
                if (appendStr == "true") append = true;
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
/*int main() {
    const std::string diskName = "cli_disk.dat";
    const size_t numBlocks = 256;

    // Initialize DiskManager and FileManager
    DiskManager diskManager(diskName, numBlocks);
    FileManager fileManager(diskManager);

    // Start the CLI
    startCLI(fileManager);

    return 0;
}*/
#endif