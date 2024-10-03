#include "cli.h"

CLI::CLI() : node(nullptr) {}

void CLI::run() {
    std::string command;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command.substr(0, 20) == "proc node interface ")
        {
            std::string interface = command.substr(20);

            try {
                node = new Node(interface.c_str());
            } catch (const std::exception& e) {
                continue;
            }
        } else if (command.substr(0, 15) == "proc node send ") {
            if ( node == nullptr ) {
                std::cout << "proc node before!" << std::endl;
                continue;
            }
            std::string payload = command.substr(15);
            node->send_data(payload.c_str());
        } else if (command == "proc node receive") {
            if ( node == nullptr ) {
                std::cout << "proc node before!" << std::endl;
                continue;
            }
            node->receive_data();
        } else if (command == "exit") {
            break;
        } else if (command == "help") {
            display_help();
        } else if (command.empty()) {
            continue;
        } else {
            std::cout << "unknown command, enter help" << std::endl;
        }
    }

    delete node;
}

void CLI::display_help() {
    std::cout << "commands:\n"
              << " proc node interface <interface> - proc node to bind with interface\n"
              << " proc node send <message> - proc node to send message\n"
              << " proc node receive - proc node to receive message\n"
              << " exit - exit application\n";
}
