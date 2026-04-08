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

int main() {
 

    // Onload example: Create a TCP socket using Onload for high-performance networking
    std::cout << "\n--- Onload Networking Example ---" << std::endl;

    // Create an Onload-accelerated TCP socket
    int sock = onload_socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Failed to create Onload socket: " << std::strerror(errno) << std::endl;
        std::cerr << "Note: Onload library may not be installed or configured." << std::endl;
        return 1;
    }

    std::cout << "Successfully created Onload-accelerated TCP socket (fd: " << sock << ")." << std::endl;

    // Set up sockaddr_in for binding (example: bind to any address on port 0 for demonstration)
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(0);  // Let system assign a port

    // Bind the socket
    if (onload_bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        std::cerr << "Failed to bind Onload socket: " << std::strerror(errno) << std::endl;
        close(sock);
        return 1;
    }

    std::cout << "Successfully bound Onload socket." << std::endl;

    // Clean up
    if (close(sock) == -1) {
        std::cerr << "Failed to close socket: " << std::strerror(errno) << std::endl;
        return 1;
    }

    std::cout << "Onload socket closed successfully." << std::endl;

    return 0;
}
