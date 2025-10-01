#include <cstring>
#include <iostream>

#include <netinet/in.h> // sockaddr_in, htons, etc.
#include <sys/socket.h> // socket(), connect(), send(), etc.
#include <unistd.h>     // close()

int main(int argc, char* argv[])
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;          // IPv4 endpoint structure
    std::memset(&serverAddress, 0, sizeof(serverAddress)); // zero it for safety

    serverAddress.sin_family = AF_INET; // Address family: IPv4
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    const char* message = "Hello, server!";
    if (argc > 1) {
        // If an argument is provided, use it as the message.
        message = argv[1];
    }

    send(clientSocket, message, std::strlen(message), 0);

    close(clientSocket);

    return 0;
}