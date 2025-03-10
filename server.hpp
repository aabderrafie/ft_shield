#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
// ANSI color codes


#define PORT 80
#define MAX_CLIENTS 1024

class Server {
public:
    Server();
    ~Server();
    void start();
    void handle_connections();

private:
    int server_fd;
    std::vector<int> clients;
    struct sockaddr_in address;
    int addrlen;
    std::mutex clients_mutex; // Mutex to protect the clients vector

    void handle_client(int client_socket);
    int authenticate(int client_socket);
    void bind_and_listen();
};