#include "Server.h"
#include <asio.hpp>

crow::SimpleApp Server::app;
std::vector<std::string> Server::sync_files_list;
std::mutex Server::sync_files_mutex;

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
            std::lock_guard<std::mutex> lock(sync_files_mutex);
            sync_files_list.push_back(ip);
        }

        return crow::response(200, "IP added successfully");
    });

    CROW_ROUTE(app, "/get_list")([]() {
        std::string result;
        for (const std::string& ip : sync_files_list) {
            result += (ip + ",");
        }

        crow::json::wvalue res;
        res["ip"] = result;
        return crow::response(res);
    });

    try {
        app.bindaddr(ADDR).port(443).multithreaded().run();
    } catch (const std::system_error& e) {
        std::cerr << "Failed to start server: " << e.what() << std::endl;
    }
}
