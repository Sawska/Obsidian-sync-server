#ifndef SERVER_H
#define SERVER_H

#include "crow.h"
#include <queue>

class Server {
    public:
    static crow::SimpleApp app;
    static std::vector<std::string> sync_files_list;
    static std::mutex sync_files_mutex;

    void start_server();
};

#endif //Server_h