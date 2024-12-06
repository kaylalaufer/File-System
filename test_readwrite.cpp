
#include <iostream>
#include "disk_manager.h"
#include "file_manager.h"
#include <fstream>


// Initialize the disk file
void initializeDisk(const std::string& diskName) {
    std::ofstream disk(diskName, std::ios::binary | std::ios::trunc);
    std::string emptyData(MAX_BLOCKS * BLOCK_SIZE, '\0');
    disk.write(emptyData.c_str(), emptyData.size());
    disk.close();
}

int main() {
    const std::string diskName = "test_main_disk.dat";
    initializeDisk(diskName);

    try {
        // Initialize DiskManager and FileManager
        DiskManager diskManager(diskName, MAX_BLOCKS);
        FileManager fileManager(diskManager);

        // Create a file
        std::cout << "Creating file '/file.txt'...\n";
        fileManager.createFile("/file.txt", 0);

        // Write data to the file
        std::string data1 = "Hello, ";
        std::string data2 = "World!";
        std::cout << "Writing data to '/file.txt'...\n";
        fileManager.writeFile("/file.txt", data1, false);  // Overwrite
        fileManager.writeFile("/file.txt", data2, true);   // Append

        // Read data from the file
        std::cout << "Reading data from '/file.txt'...\n";
        std::string result = fileManager.readFile("/file.txt");
        std::cout << "Read data: " << result << "\n";

        // Overwrite the file
        std::string newData = "Data: Overwritten data";
        std::cout << "Overwriting '/file.txt'...\n";
        fileManager.writeFile("/file.txt", newData, false);

        // Read data after overwriting
        std::cout << "Reading data after overwriting '/file.txt'...\n";
        result = fileManager.readFile("/file.txt");
        std::cout << "Read data: " << result << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}