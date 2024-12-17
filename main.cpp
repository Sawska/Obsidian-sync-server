#include "Server.h"
#include "ClientFile.h"
#include "ServerFile.h"
#include <iostream>
// #include "SyncFileReader.h" 

int main() {
    std::cout << "Welcome to the Obsidian Sync program" << std::endl;

    int choice = 0;
    while (true) {
        std::cout << "\nPlease choose an option:" << std::endl;
        std::cout << "1) Run the sync server" << std::endl;
        std::cout << "2) Run the server to receive files" << std::endl;
        std::cout << "3) Run the client to send files" << std::endl;
        std::cout << "4) Exit" << std::endl;

        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear(); 
            std::cin.ignore(1000, '\n');
            std::cerr << "Invalid input. Please enter a number between 1 and 4." << std::endl;
            continue;
        }

        switch (choice) {
            case 1: {
                Server server;
                std::cout << "Starting the sync server..." << std::endl;
                server.start_server();
                break;
            }
            case 2: {
                ServerFile serverFile;
                std::cout << "Starting the server to receive files..." << std::endl;
                serverFile.receiveFiles(OB_PATH);
                break;
            }
            case 3: {
                std::string serverIP;
                int port;
                std::string directoryPath;

                std::cout << "Enter the server IP address: ";
                std::cin >> serverIP;

                std::cout << "Enter the port number: ";
                std::cin >> port;

                std::cout << "Enter the directory path to send files: ";
                std::cin >> directoryPath;

                try {
                    Client client(serverIP, port);
                    std::cout << "Sending files from: " << directoryPath << std::endl;
                    client.sendFiles(directoryPath);
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
            case 4:
                std::cout << "Exiting the program. Goodbye!" << std::endl;
                return 0;
            default:
                std::cerr << "Invalid option. Please enter a number between 1 and 4." << std::endl;
        }
    }

    return 0;
}
