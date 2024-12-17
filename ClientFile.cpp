#include "ClientFile.h"


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

void Client::sendMessage(const std::string &message)
{
    if (send(clientSocket,message.c_str(),message.size(),0) == -1) {
        throw std::runtime_error("Failed to send message");
    }
    std::cout << "Message sent:" << message << std::endl;
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
}



void Client::sendFiles(const std::string& directoryPath) {
    std::vector<std::string> files = read_files(directoryPath);

    for (const auto& filePath : files) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            continue;
        }
        std::string fileContent((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

        std::string fileName = std::__fs::filesystem::path(filePath).filename().string();
        if (send(clientSocket, fileName.c_str(), fileName.size(), 0) == -1) {
            std::cerr << "Failed to send file name: " << fileName << std::endl;
            continue;
        }

        char delimiter = '\0';
        send(clientSocket, &delimiter, sizeof(delimiter), 0);

        if (send(clientSocket, fileContent.c_str(), fileContent.size(), 0) == -1) {
            std::cerr << "Failed to send file content: " << fileName << std::endl;
            continue;
        }

        std::cout << "File sent successfully: " << filePath << std::endl;
    }
}


