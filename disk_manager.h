// disk_manager.h
#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <string>

extern const size_t BLOCK_SIZE; // Add this if it's part of your constants

void initializeDisk(const std::string& diskName, size_t diskSize);
void writeBlock(size_t blockNumber, const std::string& data, const std::string& diskName);
std::string readBlock(size_t blockNumber, const std::string& diskName);

#endif // DISK_MANAGER_H
