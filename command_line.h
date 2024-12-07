#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include "file_manager.h"

bool isSizeNumber(const std::string& str);
void startCLI(FileManager& fileManager);

#endif // COMMAND_LINE_H
