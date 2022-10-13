#include "sockets.hpp"
#include <iostream>

#define CLIENT_NAME "client1"

int main() {
    SocketHandler::openSocket(CLIENT_NAME);
    std::cout << "Socket open" << std::endl;

    SocketHandler::Message com {"2", "1", "bom dia"};

    SocketHandler::sendMessage(com);
    std::string men = com.send_to + "," + com.sent_from + "," + com.message;
    std::cout << "First message sent (" << men << ")" << std::endl;

    com = {"\0", "\0", "\0"};
    
    while (1) {
        if (SocketHandler::listenClient(&com) == 0){
            printf("Server died\n");
            return -1;
        }

        if (com.send_to != "\0") {
            std::cout << "Received from " << com.sent_from << ": " << com.message << std::endl;

            com.send_to = "2";
            com.sent_from = "1";

            SocketHandler::sendMessage(com);
            std::cout << "Sent (" << com.message << ") to " << com.send_to << std::endl;
            
            com = {"\0", "\0", "\0"};
        }
    }
}