#include "Server.h"
crow::SimpleApp Server::app;
std::vector<std::string> Server::sync_files_list;

void Server::start_server()
{
    CROW_ROUTE(app, "/")([]() {
        auto page = crow::mustache::load_text("main.html");
        return crow::response(page);
    });
    CROW_ROUTE(app, "/sync_files").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
    std::string ip = req.get_header_value("X-FORWARDED-FOR");
    if (ip.empty()) {
        ip = req.remote_ip_address;
    }

    if (!ip.empty()) {
        Server::sync_files_list.push_back(ip);
    }

    return crow::response(200, "IP added successfully");
});


    CROW_ROUTE(app, "/get_list")([]() {
        std::string result;
        for (const std::string& ip : Server::sync_files_list) {
            result += (ip + ",");
        }

        crow::json::wvalue res;
        res["ip"] = result;
        return crow::response(res);
    });

    app.bindaddr("192.168.1.2").port(443).multithreaded().run();
}

