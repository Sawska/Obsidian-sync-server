#ifndef SYNC_FILE_READER_H
#define SYNC_FILE_READER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

#if defined(__linux__)
#define OB_PATH "/home/user/your_path"
#elif _WIN32
#define OB_PATH "C:\\your\\windows\\path"
#else 
#define OB_PATH "/Users/alexanderkorzh/Documents/Obsidian Vault"
#endif


std::vector<std::string> read_files(const std::string& directoryPath);


void write_file(const std::string& filePath, const std::string& content);

#endif // SYNC_FILE_READER_H
