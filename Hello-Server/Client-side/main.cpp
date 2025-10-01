// code taken from: https://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/
// docs from: https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html


// These headers provide memory/string helpers and IO
#include <cstring>
#include <iostream>

// Networking headers for IPv4 addresses and sockets
#include <netinet/in.h> // sockaddr_in, htons, etc.
#include <sys/socket.h> // socket(), connect(), send(), etc.
#include <unistd.h>     // close()

int main(int argc, char* argv[])
{
    // ===========================
    // 1) Create a socket
    // ===========================
    // socket(domain, type, protocol)
    // - domain: AF_INET for IPv4 (AF_INET6 for IPv6)
    // - type:
    //   - SOCK_STREAM: reliable, connection-oriented stream (TCP)
    //   - SOCK_DGRAM: datagrams (UDP)
    // - protocol: 0 lets the OS choose the default for the given type
    //   (for AF_INET + SOCK_STREAM this becomes IPPROTO_TCP)
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // NOTE: In production code, always check for errors:
    // if (clientSocket == -1) { perror("socket"); return 1; }

    // ===========================
    // 2) Specify the server address we want to connect to
    // ===========================
    sockaddr_in serverAddress;          // IPv4 endpoint structure
    std::memset(&serverAddress, 0, sizeof(serverAddress)); // zero it for safety

    serverAddress.sin_family = AF_INET; // Address family: IPv4
    serverAddress.sin_port = htons(8080);
    // htons = host-to-network short (16-bit). Network byte order is big-endian.

    // IMPORTANT:
    // INADDR_ANY is used by SERVERS when binding to "any" local interface.
    // It is NOT appropriate for clients when connecting to a remote.
    // Using it here sets the destination to 0.0.0.0, which is not a valid remote.
    //
    // To connect to a local server on the same machine, you usually want 127.0.0.1.
    // Example (preferred):
    //   #include <arpa/inet.h>
    //   inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
    //
    // We keep your original line to preserve behavior, but note it won't connect unless
    // the destination is set to a real IP address.
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // ===========================
    // 3) Connect to the server
    // ===========================
    // connect() initiates a TCP handshake to the specified address:port.
    // This call blocks until either the connection is established or fails.
    // Returns 0 on success, -1 on error.
    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // Again, in robust code, check:
    // if (connect(...) == -1) { perror("connect"); close(clientSocket); return 1; }

    // ===========================
    // 4) Prepare the message to send
    // ===========================
    const char* message = "Hello, server!";
    if (argc > 1) {
        // If an argument is provided, use it as the message.
        message = argv[1];
    }

    // ===========================
    // 5) Send data to the server
    // ===========================
    // send(fd, buffer, length, flags)
    // Returns number of bytes sent, or -1 on error.
    // Note: With TCP, send may send fewer bytes than requested. For small strings,
    // it's usually fine, but for robustness, loop until all bytes are sent.
    send(clientSocket, message, std::strlen(message), 0);

    // ===========================
    // 6) Close the socket
    // ===========================
    // Close our end of the connection. For TCP, this initiates a FIN.
    // The kernel will handle the proper shutdown handshake with the peer.
    close(clientSocket);

    return 0;
}