#ifndef CLIENTFILE_H
#define CLIENTFILE_H

#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <arpa/inet.h>
#include <fstream>
#include "SyncFileReader.h"
#include <filesystem>

class Client {
private: 
    int clientSocket;
    sockaddr_in serverAdrress;
public:
    Client(const std::string& serverIP,int port);
    ~Client();
    void sendMessage(const std::string& message);
    void sendFile(const std::string &filePath);
    void sendFiles(const std::string& directoryPath);
    void Client::receiveAcknowledgment();
};

#endif //CLIENTFILE_H