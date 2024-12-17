#ifndef SERVERFILE_H
#define SERVERFILE_H

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <fstream>
#include "SyncFileReader.h"

class ServerFile {
private:
    int serverSocket;
    int clientSocket;
    sockaddr_in serverAddress;

public:
    ServerFile(int port = 8080);
    ~ServerFile();
    void acceptConnection();
    std::string receiveData();
    void closeConnection();
    void receiveFile(const std::string &outputFilePath);
    void receiveFiles(const std::string& outputDirectory);
};

#endif //SERVERFILE_H