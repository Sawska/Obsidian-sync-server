#ifndef SERVER_H
#define SERVER_H

#include "crow.h"
#include <queue>
#include <mutex>
#include <string>
#include <vector>

#if defined(__linux__)
#define ADDR "127.0.0.1"
#elif defined(_WIN32)
#define ADDR "127.0.0.1"  // Default to localhost for Windows
#elif defined(__APPLE__)
#define ADDR "192.168.1.2"  // Use the correct address for Mac
#else
#define ADDR "127.0.0.1"  // Default to localhost for other OS
#endif

class Server {
public:
    static crow::SimpleApp app;
    static std::vector<std::string> sync_files_list;
    static std::mutex sync_files_mutex;

    void start_server();
};

#endif // SERVER_H
