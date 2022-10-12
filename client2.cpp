#include "sockets.hpp"
#include <iostream>

#define CLIENT_NAME "client2"

int main() {
    SocketHandler::openSocket(CLIENT_NAME);
    std::cout << "Socket open" << std::endl;

    SocketHandler::Message com {"1", "2", "bom dia"};

    SocketHandler::sendMessage(com);
    std::cout << "First message sent" << std::endl;

    SocketHandler::Message null;

    while (1) {
        //SocketHandler::listenClient(&com);
        //std::cout << "Client listened" << std::endl;

        SocketHandler::listenClient(&null);
        SocketHandler::sendMessage(com);
        std::cout << "Sent (" << com.message << ") to " << com.send_to << std::endl;
        /*
        if (com.send_to == "2") {
            std::cout << "Received from " << com.sent_from << ": " << com.message << std::endl;

            com.send_to = com.sent_from;
            com.sent_from = 2;

            SocketHandler::sendMessage(com);
            std::cout << "Sent (" << com.message << ") to " << com.send_to << std::endl;
            
            com = {"\0", "\0", "\0"};
        }*/
    }
    
    // SocketHandler::closeSocket(SocketHandler::connection_socket);
}