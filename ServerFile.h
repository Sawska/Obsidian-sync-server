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
#include <arpa/inet.h>  
#include <cstring>      
#include <stdexcept>    
#include <iostream>     


#if defined(__linux__)
#define ADDR "192.168.1.7"
#elif defined(_WIN32)
#define ADDR "127.0.0.1"  // Default to localhost for Windows
#elif defined(__APPLE__)
#define ADDR "192.168.1.2"  // Use the correct address for Mac
#else
#define ADDR "192.168.1.7"  // Default to localhost for other OS
#endif

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
    void sendAcknowledgment(const std::string &message);
};

#endif //SERVERFILE_H