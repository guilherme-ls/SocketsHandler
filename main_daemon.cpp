#include "sockets.hpp"
#include <iostream>

int main() {
    const int connection_number = 3;

    std::string connection_list[connection_number] = {"client1", "client2", "client3"};

    int* client_sockets = (int*)calloc(connection_number, sizeof(int));

    // Setups server
    SocketHandler::start();
    std::cout << "Server initialized" << std::endl;

    SocketHandler::Message server_com {"\0", "\0", "\0"};
    
    // Listens and replies
    while (1) {
        SocketHandler::listenServer(client_sockets, connection_list, connection_number, &server_com);
        std::cout << "Server listened" << std::endl;

        if (server_com.message != "\0") {
            // Do something to be defined (server commands)
        }

        server_com = {"\0", "\0", "\0"};
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