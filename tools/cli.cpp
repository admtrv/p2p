/*
 *  cli.cpp
 */

#include "cli.h"

CLI::CLI(Node* node) : node(node) {}

void CLI::run() {
    std::string command;
    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command.substr(0, 14) == "proc node inf ")
        {
            std::string interface = command.substr(14);

            try {
                node->init_inf(interface.c_str());
            } catch (const std::exception& e) {
                continue;
            }
        }

        else if (command.substr(0, 15) == "proc node port ")
        {
            int port = std::stoi(command.substr(15));

            node->set_port(port);
        }

        else if (command.substr(0, 15) == "proc node dest ")
        {
            std::string ip_and_port = command.substr(15);

            size_t separator = ip_and_port.find(' ');

            if (separator != std::string::npos)
            {
                std::string ip = ip_and_port.substr(0, separator);
                int port = std::stoi(ip_and_port.substr(separator + 1));

                node->set_dest(ip, port);
            }
        }

        else if (command == "proc node connect")
        {
            node->send_arp_request();
        }

        else if (command == "exit")
        {
            node->stop_receiving();
            break;
        }

        else if (command == "help")
        {
            display_help();
        }

        else if (command.empty())
        {
            continue;
        }

        else
        {
            std::cout << "unknown command, enter help" << std::endl;
        }
    }
}

void CLI::display_help() {
    std::cout << "commands:\n"
              << " proc node inf <interface> - bind node with interface\n"
              << " proc node port <port> - set port node will listen\n"
              << " proc node dest <ip> <port> - set destination node ip and port\n"
              << " proc node connect - sends ARP request\n"
              << " exit - exit application\n";
}
