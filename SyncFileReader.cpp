#include "SyncFileReader.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#if defined(__linux__)
namespace fs = std::filesystem;
#elif defined(_WIN32)
namespace fs = std::filesystem;
#elif defined(__APPLE__)
namespace fs = std::__fs::filesystem;
#endif

std::vector<std::string> read_files(const std::string& directoryPath) {
    std::vector<std::string> filePaths;

    for (const auto& entry : fs::recursive_directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            filePaths.push_back(entry.path().string());
        }
    }

    return filePaths;
}

void write_file(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    file.write(content.c_str(), content.size());
    file.close();
}
