#include "sockets.hpp"
#include <iostream>

#define CLIENT_NAME "client1"

int main() {
    SocketHandler::openSocket(CLIENT_NAME);
    std::cout << "Socket open" << std::endl;

    SocketHandler::Message com {"\0", "\0", "\0"};
    
    while (1) {
        SocketHandler::listenClient(&com);

        if (com.send_to == "1") {
            std::cout << "Received from " << com.sent_from << ": " << com.message << std::endl;

            com.send_to = 2;
            com.sent_from = 1;

            SocketHandler::sendMessage(com);
            std::cout << "Sent (" << com.message << ") to " << com.send_to << std::endl;
            
            com = {"\0", "\0", "\0"};
        }
    }
    
    // SocketHandler::closeSocket(SocketHandler::connection_socket);
}