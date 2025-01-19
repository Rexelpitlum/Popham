#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> // for strlen

#define RX_BUFFER_SIZE 1024
#define RX_PORT 54487

int main() {
    int sockFd, bytesReceived;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char rxBuf[RX_BUFFER_SIZE];

    // Create a UDP socket
    sockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockFd < 0)
    {
        std::cerr << "socket creation failed" << std::endl;
        return -1;
    }

    // Set up the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(RX_PORT);
    inet_pton(AF_INET, "0.0.0.0", &serverAddr.sin_addr);

    // Bind the socket to the interface
    if (bind(sockFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "bind failed" << std::endl;
        return 1;
    }

    std::cout << "Listening on port " << RX_PORT << "...\n";

    while (true)
    {
        // Receive a packet
        bytesReceived = recvfrom(sockFd, rxBuf, RX_BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (bytesReceived < 0)
        {
            std::cerr << "recvfrom failed" << std::endl;
            continue;
        }

        // Process the packet contents
        std::cout << "Received " << bytesReceived << " bytes from client IP " << inet_ntoa(clientAddr.sin_addr)
                  << ", port " << ntohs(clientAddr.sin_port) << std::endl;

        // Send a reply to the client
        const char* reply = "Hello, client!";
        sendto(sockFd, reply, strlen(reply), 0, (struct sockaddr *)&clientAddr, clientAddrLen);
    }

    return 0;
}
