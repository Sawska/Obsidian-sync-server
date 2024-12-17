#include "SyncFileReader.h"
#include <filesystem>

namespace fs = std::__fs::filesystem;

std::vector<std::string> read_files(const std::string& directoryPath) {
    std::vector<std::string> filePaths;

    for (const auto& entry : fs::recursive_directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) { // Check if it's a file
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
