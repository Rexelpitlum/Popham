#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>

#define BUFFER_SIZE 1024

void usage( const char* appName )
{
    std::cerr << "Usage: " << appName << "  <address> <port> <message> [timeout] (seconds)" << std::endl;
    exit(-1);
}

int main(int argc, char* argv[])
{
    if (argc < 4 || argc > 5) {
        usage( argv[0] );
    }

    const char* destAddress = argv[1];
    int destPort = std::stoi(argv[2]);
    const char* message = argv[3];

    int timeout = 5;  // Default timeout: 5 seconds
    if (argc == 5)
    {
        timeout = std::stoi(argv[4]);
    }

    int sockFd;
    struct sockaddr_in destSockAddr;
    socklen_t destSockLen = sizeof(destSockAddr);

    // Create a UDP socket
    sockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockFd < 0)
    {
        std::cerr << "socket creation failed" << std::endl;
        return -1;
    }

    // Set up the server address
    destSockAddr.sin_family = AF_INET;
    destSockAddr.sin_port = htons(destPort);
    if (inet_pton(AF_INET, destAddress, &destSockAddr.sin_addr) <= 0)
    {
        std::cerr << "invalid address" << std::endl;
        return -1;
    }

    // Send the message
    if (sendto(sockFd,
                message,
                strlen(message),
                0,
                (struct sockaddr*)&destSockAddr,
                destSockLen) < 0)
    {
        std::cerr << "sendto failed" << std::endl;
        return -1;
    }

    std::cout << "Sent message: " << message << std::endl;

    // Wait for a response with a timeout
    fd_set rxFds;
    FD_ZERO(&rxFds);
    FD_SET(sockFd, &rxFds);

    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    int result = select(sockFd + 1, &rxFds, NULL, NULL, &tv);
    if (result < 0)
    {
        std::cerr << "select failed" << std::endl;
        return -1;
    }
    else if (result == 0)
    {
        std::cout << "Timeout: no response received" << std::endl;
        return -1;
    }

    // Receive the response
    char buffer[BUFFER_SIZE];
    int bytesReceived = recvfrom(sockFd,
                                buffer,
                                BUFFER_SIZE-1,
                                0,
                                (struct sockaddr*)&destSockAddr,
                                &destSockLen);
    if (bytesReceived < 0)
    {
        std::cerr << "recvfrom failed" << std::endl;
        return -1;
    }

    if (bytesReceived < BUFFER_SIZE)
    {
        buffer[bytesReceived] = '\0';  // Null-terminate the buffer
    }
    else
    {
        std::cerr << "Buffer overflow detected" << std::endl;
        return -1;
    }

    std::cout << "Received response: " << buffer << std::endl;

    close(sockFd);
    return 0;
}
