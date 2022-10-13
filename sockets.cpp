#include "sockets.hpp"

int SocketHandler::connection_socket;

// Starts main daemon's socket
// return value: -1 = failed; 0 = succeded
int SocketHandler::start() {
    unlink(SOCKET_NAME);

    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));

    // Creates a local main socket
    connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection_socket == -1) {
        perror("socket");
        return -1;
    }

    // Binds socket to defined address
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_NAME, sizeof(address.sun_path) - 1);
    if (bind(connection_socket, (const struct sockaddr *) &address, sizeof(address)) == -1) {
        perror("bind");
        return -1;
    }

    // Prepares for accepting connections
    if (listen(connection_socket, 20) == -1) {
        perror("listen");
        return -1;
    }

    return 0;
}

// Starts client sockets
// return value: -1 = failed; 0 = succeded
int SocketHandler::openSocket(std::string name) {
    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));

    // Creates local socket
    connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection_socket == -1) {
        perror("socket");
        return -1;
    }

    // Connects to main_daemon's socket adress
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_NAME, sizeof(address.sun_path) - 1);

    if (connect(connection_socket, (const struct sockaddr *) &address, sizeof(address)) == -1) {
        perror("The server is down");
        return -1;
    }

    // Sends socket information to main    
    write(connection_socket, name.c_str(), name.length());

    // Confirmation
    char buffer[5] = "null";
    while(strcmp(buffer, "rcvd") != 0) {
        read(connection_socket, buffer, 5);
    }
    return 0;
}

// Client main listening function
// return value: 1 = success; 0 = server down; -1 = local socket failure
int SocketHandler::listenClient(SocketHandler::Message* com) {
    // Puts all sockets on list
    fd_set fd_reads;
    FD_ZERO(&fd_reads);
    FD_SET(connection_socket, &fd_reads);
    
    struct timeval time = {1,0};
    // Checks wich sockets can be read
    ssize_t activity = select(connection_socket + 1, &fd_reads, NULL, NULL, &time);       
    if ((activity < 0) && (errno!=EINTR)) {  
        printf("select error");
        return -1;
    }

    // Reads message, if present
    if (FD_ISSET(connection_socket, &fd_reads)) {
        int temp;
        char buffer[STD_SIZE];

        if((temp = read(connection_socket, buffer, STD_SIZE)) == -1) {
            perror("read");
            return -1;
        }
        else if (temp == 0) {
            close(connection_socket);
            return 0;
        }
        else {
            buffer[temp] = '\0';

            // Returns message
            *com = SocketHandler::strToMsg(buffer);
        }
    }

    return 1;
}

// Server listening function
// still needs to implement an error system
void SocketHandler::listenServer(int* client_sockets, std::string* connection_list, int size, SocketHandler::Message* server_com) {
    // int ret_val = 1;
    // Puts all sockets on list
    int max_fd = connection_socket;
    fd_set fd_reads;
    FD_ZERO(&fd_reads);
    FD_SET(connection_socket, &fd_reads);
    for (int i = 0; i < size; i++) {
        if (client_sockets[i] != 0) {
            FD_SET(client_sockets[i], &fd_reads);
            if (client_sockets[i] > max_fd) {
                max_fd = client_sockets[i];
            }
        }
    }

    struct timeval time = {1,0};
    // Checks wich sockets can be read
    ssize_t activity = select(max_fd + 1, &fd_reads, NULL, NULL, &time);       
    if ((activity < 0) && (errno!=EINTR)) {
        printf("select error");
        // return -1;
    }

    // If main socket can be read, accepts new connections
    if (FD_ISSET(connection_socket, &fd_reads)) {
        int temp;
        // Accepts
        if ((temp = accept(connection_socket, NULL, NULL)) == -1) {
            perror("accept");
            // return = -1;
        }
        
        // Gets connected socket's identity
        char buffer[STD_SIZE];
        read(temp, buffer, STD_SIZE);

        for (int i = 0; i < size; i++) {
            if (strcmp(buffer, connection_list[i].c_str()) == 0) {
                client_sockets[i] = temp;
                printf("Stored new connection\n");  
            }
        }

        write(temp, "rcvd", 5); 
    }

    // Transfers messages between children
    for (int i = 0; i < size; i++) {
        if (FD_ISSET(client_sockets[i], &fd_reads)) {
            int signal;
            char buffer[STD_SIZE];
            if ((signal = read(client_sockets[i], buffer, STD_SIZE)) == -1) {
                perror("read");
                // return = -1;
            }
            // Socket closing signal
            else if (signal == 0) {
                close(client_sockets[i]);
                client_sockets[i] = 0;
                // Can include code to inform main of the event
            }
            // Transfers message or receives it
            else {
                buffer[signal] = '\0';
                SocketHandler::Message com = strToMsg(buffer);

                if (std::stoi(com.send_to) < 0 || std::stoi(com.send_to) > size) {
                    printf("invalid destination");
                    // return = -1;
                }
                else if (std::stoi(com.send_to) == 0) {
                    *server_com = com;
                    // return 0;
                }
                else {
                    transfer(com, client_sockets, size);
                    printf("Transfered message\n");
                    // return 1;
                }                
            }
        }
    }
    // return 1;
}

//Send message from client to server
// return value: -1 = failed; 0 = succeded
int SocketHandler::sendMessage(SocketHandler::Message com) {
    std::string men = com.send_to + "," + com.sent_from + "," + com.message;

    if (write(connection_socket, men.c_str(), men.length()) == -1) {
        perror("message not sent");
        return -1;
    }
    return 0;
}

// Transfers messages between clients
// return value: -1 = failed; 0 = succeded
int SocketHandler::transfer(SocketHandler::Message com, int* client_sockets, int size) {
    std::string men = com.send_to + "," + com.sent_from + "," + com.message;

    if (write(client_sockets[std::stoi(com.send_to)-1], men.c_str(), men.length()) == -1) {
        perror("message not sent");
        return -1;
    }
    return 0;
}

// Converts string to Message struct
SocketHandler::Message SocketHandler::strToMsg(char* com) {
    std::string cut[3];
    int i = 0, j = 0;
    while (com[i] != '\0' && j < 3) {
        if (com[i] != ',') {
            cut[j] += com[i];
        }
        else {
            j++;
        }
        i++;
    }

    SocketHandler::Message mes {cut[0], cut[1], cut[2]};

    return mes;
}

void SocketHandler::closeSocket(int dis_socket) {
    struct sockaddr_un name;
    getpeername(dis_socket, (struct sockaddr *) &name, (socklen_t *) sizeof(name));
    close(dis_socket);
    unlink(name.sun_path);
}





/*
int NO(int argc, char *argv[]) {
    struct sockaddr_un name;
    int down_flag = 0;
    int connection_socket;
    int data_socket;
    char buffer[STD_SIZE];

    // Create local socket
    connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&name, 0, sizeof(name));

    // Bind socket to socket name
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
    if (bind(connection_socket, (const struct sockaddr *) &name, sizeof(name)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Prepare for accepting connections
    if (listen(connection_socket, 20) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
        
    while (1) {
        // Wait for incoming connection
        data_socket = accept(connection_socket, NULL, NULL);
        if (data_socket == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        while (1) {
            // Wait for next data packet
            if (read(data_socket, buffer, sizeof(buffer)) == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            // Ensure buffer is 0-terminated
            buffer[sizeof(buffer) - 1] = 0;

            // Stops reading when receives END signal
            if (!strncmp(buffer, "END", sizeof(buffer))) {
                break;
            }

            // Kills server when receives DOWN signal
            if (!strncmp(buffer, "DOWN", sizeof(buffer))) {
                down_flag = 1;
                break;
            }

            printf("received = %s\n", buffer);

            // Send result back
            if (write(data_socket, buffer, sizeof(buffer)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }

            printf("sent = %s\n", buffer);
        }        

        // Close socket
        close(data_socket);

        // Quit on DOWN command
        if (down_flag) {
            printf("Server closed\n");
            break;
        }
    }

    close(connection_socket);

    // Unlink the socket
    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);

}
*/