#include "sockets.hpp"
#include <iostream>

int main() {
    const int connection_number = 2;

    std::string connection_list[connection_number] = {"client1", "client2"};

    int* client_sockets = (int*)calloc(connection_number, sizeof(int));

    // Setups server
    SocketHandler::start();
    std::cout << "Server initialized" << std::endl;
    
    // Listens and replies
    for (int i = 0; i < 20; i++) {
        SocketHandler::listenServer(client_sockets, connection_list, connection_number);
        std::cout << "Server listened" << std::endl;
    }
    
    std::cout << "Loop stopped" << std::endl;

    // Closes all clients
    for (int i = 0; i < connection_number; i++) {
        if (client_sockets[i] != 0) {
            SocketHandler::closeSocket(client_sockets[i]);
        }
    }
    // Closes server socket
    SocketHandler::closeSocket(SocketHandler::connection_socket);
    free(client_sockets);
    std::cout << "Server closed" << std::endl;
}