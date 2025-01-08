#include "ClientFile.h"
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



Client::Client(const std::string &serverIP, int port) : clientSocket(-1) 
{
    clientSocket = socket(AF_INET,SOCK_STREAM,0);
    if (clientSocket == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    serverAdrress.sin_family = AF_INET;
    serverAdrress.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIP.c_str(), &serverAdrress.sin_addr) <= 0) {
            close(clientSocket);
            throw std::runtime_error("Invalid server IP address");
        }

        if(connect(clientSocket,(struct sockaddr*)&serverAdrress,sizeof(serverAdrress)) == -1) {
            close(clientSocket);
            throw std::runtime_error("Failed to connect to server");
        }
        std::cout << "Connected to server at " << serverIP << ":" << port << std::endl;
}

Client::~Client()
{
    close(clientSocket);
}

void Client::receiveAcknowledgment() {
    char buffer[1024] = {0};
ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
if (bytesReceived > 0) {
    buffer[bytesReceived] = '\0';
    std::cout << "Received acknowledgment: " << buffer << std::endl;
} else if (bytesReceived == 0) {
    std::cerr << "Server closed the connection." << std::endl;
} else {
    std::cerr << "Error receiving acknowledgment: " << strerror(errno) << std::endl;
}
}


void Client::sendFile(const std::string &filePath) {

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    char buffer[1024];
    while (!file.eof()) {
        file.read(buffer, sizeof(buffer));
        std::streamsize bytesRead = file.gcount();

        if (send(clientSocket, buffer, bytesRead, 0) == -1) {
            file.close();
            throw std::runtime_error("Failed to send file data");
        }
    }
    file.close();
    std::cout << "File sent successfully: " << filePath << std::endl;
    receiveAcknowledgment();
}



void Client::sendFiles(const std::string& directoryPath) {
    std::vector<std::string> files = read_files(directoryPath);

    for (const auto& filePath : files) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            continue;
        }

        std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        uint64_t fileSize = fileContent.size();

        fs::path fullPath(filePath);
        fs::path basePath(directoryPath);
        std::string relativePath = fs::relative(fullPath, basePath).string();
        uint64_t relativePathSize = relativePath.size();

        if (send(clientSocket, &relativePathSize, sizeof(relativePathSize), 0) == -1) {
            std::cerr << "Failed to send relative path size for: " << relativePath << std::endl;
            continue;
        }
        if (send(clientSocket, relativePath.c_str(), relativePath.size(), 0) == -1) {
            std::cerr << "Failed to send relative path: " << relativePath << std::endl;
            continue;
        }
        if (send(clientSocket, &fileSize, sizeof(fileSize), 0) == -1) {
            std::cerr << "Failed to send file size for: " << relativePath << std::endl;
            continue;
        }
        if (send(clientSocket, fileContent.c_str(), fileContent.size(), 0) == -1) {
            std::cerr << "Failed to send file content: " << relativePath << std::endl;
            continue;
        }

        std::cout << "File sent successfully: " << filePath << " as " << relativePath << std::endl;

        try {
            receiveAcknowledgment();  
        } catch (const std::exception& e) {
            std::cerr << "Acknowledgment error for file " << relativePath << ": " << e.what() << std::endl;
        }
    }

    uint64_t terminationSignal = 0;
    if (send(clientSocket, &terminationSignal, sizeof(terminationSignal), 0) == -1) {
        std::cerr << "Failed to send end-of-transmission signal" << std::endl;
    } else {
        std::cout << "End-of-transmission signal sent" << std::endl;
    }
    shutdownConnection();
}



void Client::shutdownConnection() {
    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
    std::cout << "Connection shut down gracefully" << std::endl;
}
