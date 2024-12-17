#include "ServerFile.h"


ServerFile::ServerFile(int port) : serverSocket(-1), clientSocket(-1) 
{
    serverSocket = socket(AF_INET,SOCK_STREAM,0);
    if (serverSocket == -1) {
        throw std::runtime_error("Failed to create socket");
    }


    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket,(struct sockaddr*)&serverAddress,sizeof(serverAddress)) == -1) {
        close(serverSocket);
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(serverSocket,5) == -1) {
        close(serverSocket);
        throw std::runtime_error("Failed to listen on socket");
    }

    std::cout << "Server is listening on port" << port << std::endl;
}

ServerFile::~ServerFile()
{
    close(clientSocket);
    close(serverSocket);
}

void ServerFile::acceptConnection()
{
    clientSocket = accept(serverSocket,nullptr,nullptr);
    if (clientSocket == -1) {
        throw std::runtime_error("failed to accept connection");
    }
    std::cout << "Client connected " << std::endl;
}

std::string ServerFile::receiveData()
{
    char buffer[1024] = {0};
    size_t bytesReceived = recv(clientSocket,buffer,sizeof(buffer),0);
    if (bytesReceived == -1) {
        throw std::runtime_error("Failed to receive data");
    }
    return std::string(buffer,bytesReceived);
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
    }
}