#include "ServerFile.h"

ServerFile::ServerFile(int port) : serverSocket(-1), clientSocket(-1)
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    // Проверка корректности адреса
    if (inet_pton(AF_INET, ADDR, &serverAddress.sin_addr) <= 0) {
        close(serverSocket);
        throw std::runtime_error("Invalid address: " ADDR);
    }

    // Привязка сокета
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        close(serverSocket);
        throw std::runtime_error("Binding failed: " + std::string(strerror(errno)));
    }

    // Прослушивание соединений
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

// Send acknowledgment to the client
void ServerFile::sendAcknowledgment(const std::string &message)
{
    if (send(clientSocket, message.c_str(), message.size(), 0) == -1) {
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

    // Send acknowledgment back to the client
    sendAcknowledgment("File received successfully: " + outputFilePath);
}

void ServerFile::receiveFiles(const std::string& outputDirectory) {
    while (true) {
        char fileNameBuffer[256] = {0};
        ssize_t bytesReceived = recv(clientSocket, fileNameBuffer, sizeof(fileNameBuffer) - 1, 0);
        if (bytesReceived <= 0) {
            break;
        }

        std::string fileName(fileNameBuffer);
        
        char delimiter;
        recv(clientSocket, &delimiter, sizeof(delimiter), 0);

        std::ofstream outFile(outputDirectory + "/" + fileName, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "Failed to open file for writing: " << fileName << std::endl;
            continue;
        }

        char fileBuffer[1024];
        while ((bytesReceived = recv(clientSocket, fileBuffer, sizeof(fileBuffer), 0)) > 0) {
            outFile.write(fileBuffer, bytesReceived);
        }

        outFile.close();
        std::cout << "File received and saved as: " << outputDirectory + "/" + fileName << std::endl;

        // Send acknowledgment after each file is received
        sendAcknowledgment("File " + fileName + " received successfully");
    }
}
