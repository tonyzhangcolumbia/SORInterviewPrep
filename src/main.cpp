#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <netinet/in.h>
#include <onload.h>
 //Onload 是 Xilinx（原 Solarflare）提供的一个用户空间网络栈，能够绕过内核，直接在用户空间处理网络数据包
int main() {
    // Onload example: Create a TCP socket using Onload for high-performance networking
    std::cout << "\n--- Onload Networking Example ---" << std::endl;

    // Create an Onload-accelerated TCP socket
    int sock = onload_socket(AF_INET, SOCK_STREAM, 0);
    // if (sock == -1) {
    //     std::cerr << "Failed to create Onload socket: " << std::strerror(errno) << std::endl;
    //     std::cerr << "Note: Onload library may not be installed or configured." << std::endl;
    //     return 1;
    // }
    std::cout << "Successfully created Onload-accelerated TCP socket (fd: " << sock << ")." << std::endl;

    // Set up sockaddr_in for binding (example: bind to any address on port 0 for demonstration)
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(0);  // Let system assign a port

    // Bind the socket to the address
    if (onload_bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        std::cerr << "Failed to bind Onload socket: " << std::strerror(errno) << std::endl;
        close(sock);
        return 1;
    }
    std::cout << "Successfully bound Onload socket." << std::endl;

    // 1. Listen for incoming connections
    if (onload_listen(sock, 5) == -1) {
        std::cerr << "Listen failed: " << std::strerror(errno) << std::endl;
        close(sock);
        return 1;
    }

    std::cout << "Waiting for a connection..." << std::endl;

    // 2. Accept a connection
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_sock = onload_accept(sock, reinterpret_cast<sockaddr*>(&client_addr), &client_len);

    // 3. Receive Data
    char buffer[1024];
    ssize_t bytes_received = onload_recv(client_sock, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        std::cout << "Received: " << buffer << std::endl;

        // 4. Send Data back (Echo)
        const char* response = "Message received!";
        ssize_t bytes_sent = onload_send(client_sock, response, strlen(response), 0);
        
        if (bytes_sent == -1) {
            std::cerr << "Send failed: " << std::strerror(errno) << std::endl;
        }
    } else if (bytes_received == 0) {
        std::cout << "Client disconnected." << std::endl;
    } else {
        std::cerr << "Receive failed: " << std::strerror(errno) << std::endl;
    }
    return 0;
}
