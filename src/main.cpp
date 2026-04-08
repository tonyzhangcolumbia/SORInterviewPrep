#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <string>
#include <string_view>

int main() {
    // Get the system's page size
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1) {
        std::cerr << "Failed to get page size: " << std::strerror(errno) << std::endl;
        return 1;
    }

    // Assume huge page size is 2MB (common default), but in practice, check /proc/meminfo
    const size_t huge_page_size = 2 * 1024 * 1024; // 2MB

    // Allocate memory using huge pages
    void* addr = mmap(nullptr, huge_page_size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);

    if (addr == MAP_FAILED) {
        std::cerr << "---Failed to allocate huge page memory: " << std::strerror(errno) << std::endl;
        std::cerr << "---Note: Huge pages may need to be configured in the system." << std::endl;
        return 1;
    }

    std::cout << "!! Successfully allocated " << huge_page_size << " bytes using huge pages." << std::endl;
    std::cout << "System page size: " << page_size << " bytes." << std::endl;

    // Use the allocated memory with std::string_view
    char* buffer = static_cast<char*>(addr);
    std::string message = "Hello, Huge Pages in C++17!";
    std::strncpy(buffer, message.c_str(), message.size() + 1);

    std::cout << "Message stored in huge page memory: " << buffer << std::endl;

    // Clean up
    if (munmap(addr, huge_page_size) == -1) {
        std::cerr << "Failed to unmap memory: " << std::strerror(errno) << std::endl;
        return 1;
    }

    std::cout << "Memory deallocated successfully." << std::endl;
    return 0;
}
