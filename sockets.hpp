#pragma once

#include <exception>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/10server.socket"
#define STD_SIZE 128

class SocketHandler {
    public:
        struct Message {
            std::string send_to;
            std::string sent_from;
            std::string message;

            Message() = default;

            Message(std::string send, std::string sent, std::string mess)
                : send_to { std::move(send) }
                , sent_from { std::move(sent) }
                , message { std::move(mess) }
                {};
        };

        static int connection_socket;

    public:
        static void start();

        static void openSocket(std::string name);

        static void listenClient(SocketHandler::Message* com);

        static void listenServer(int* sockets, std::string* connection_list, int size);

        static void sendMessage(Message com);

        static void transfer(Message com, int* sockets, int size);

        static Message strToMsg(char* com);

        static int getId(std::string name, std::string* name_list, int size);

        static void closeSocket(int dis_socket);
};