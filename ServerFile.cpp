#include "ServerFile.h"

ServerFile::ServerFile(int port) : serverSocket(-1), clientSocket(-1)
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, ADDR, &serverAddress.sin_addr) <= 0) {
        close(serverSocket);
        throw std::runtime_error("Invalid address: " ADDR);
    }
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        close(serverSocket);
        throw std::runtime_error("Binding failed: " + std::string(strerror(errno)));
    }
    if (listen(serverSocket, 5) == -1) {
        close(serverSocket);
        throw std::runtime_error("Listen failed: " + std::string(strerror(errno)));
    }

    std::cout << "Server is listening on address " << ADDR << " and port " << port << std::endl;
}

ServerFile::~ServerFile()
{
    close(clientSocket);
    close(serverSocket);
}

void ServerFile::acceptConnection()
{
    clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == -1) {
        throw std::runtime_error("failed to accept connection");
    }
    std::cout << "Client connected" << std::endl;
}

std::string ServerFile::receiveData()
{
    char buffer[1024] = {0};
    size_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1) {
        throw std::runtime_error("Failed to receive data");
    }
    return std::string(buffer, bytesReceived);
}


void ServerFile::sendAcknowledgment(const std::string &message)
{
    ssize_t sentBytes = send(clientSocket, message.c_str(), message.size(), 0);
if (sentBytes == -1) {
    std::cerr << "Failed to send acknowledgment" << std::endl;
} else {
    std::cout << "Sent acknowledgment: " << message << std::endl;
}
}

void ServerFile::closeConnection()
{
    close(clientSocket);
    clientSocket = -1;
}

void ServerFile::receiveFile(const std::string &outputFilePath) {
    std::ofstream file(outputFilePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open output file: " + outputFilePath);
    }

    char buffer[1024];
    while (true) {
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == 0) {
            break;
        }
        if (bytesReceived == -1) {
            file.close();
            throw std::runtime_error("Failed to receive file data");
        }
        file.write(buffer, bytesReceived);
    }
    file.close();
    std::cout << "File received and saved as: " << outputFilePath << std::endl;

    sendAcknowledgment("File received successfully: " + outputFilePath);
}

void ServerFile::receiveFiles(const std::string& outputDirectory) {
    while (true) {
        uint64_t relativePathSize = 0;
        ssize_t bytesReceived = recv(clientSocket, &relativePathSize, sizeof(relativePathSize), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Error or connection closed while receiving relative path size" << std::endl;
            break;
        }
        if (relativePathSize == 0) {
            std::cout << "End-of-transmission signal received. Closing connection." << std::endl;
            break;
        }

        std::vector<char> relativePathBuffer(relativePathSize + 1, 0);
        bytesReceived = recv(clientSocket, relativePathBuffer.data(), relativePathSize, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Error or connection closed while receiving relative path" << std::endl;
            break;
        }
        std::string relativePath(relativePathBuffer.data(), relativePathSize);
        std::cout << "Receiving file: " << relativePath << std::endl;

        uint64_t fileSize = 0;
        bytesReceived = recv(clientSocket, &fileSize, sizeof(fileSize), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Failed to receive file size" << std::endl;
            break;
        }

        std::string fullFilePath = outputDirectory + "/" + relativePath;
        create_directories_from_path(fullFilePath);

        std::ofstream outFile(fullFilePath, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "Failed to open file for writing: " << relativePath << std::endl;
            continue;
        }

        uint64_t totalBytesReceived = 0;
        char fileBuffer[1024];
        while (totalBytesReceived < fileSize) {
            bytesReceived = recv(clientSocket, fileBuffer, sizeof(fileBuffer), 0);
            if (bytesReceived <= 0) {
                if (bytesReceived == 0) {
                    std::cout << "Client closed connection" << std::endl;
                } else {
                    std::cerr << "Error receiving file data or connection closed" << std::endl;
                }
                break;
            }

            totalBytesReceived += bytesReceived;
            outFile.write(fileBuffer, bytesReceived);
        }

        outFile.close();
        std::cout << "File received and saved as: " << fullFilePath << std::endl;

        sendAcknowledgment("File " + relativePath + " received successfully");
    }
    shutdownConnection();
}



void ServerFile::shutdownConnection() {
    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
    std::cout << "Connection closed gracefully" << std::endl;
}
