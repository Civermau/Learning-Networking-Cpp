// code taken from: https://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/
// docs from: https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html

// Basics
#include <cstring>
#include <iostream>

// Networking
#include <netinet/in.h> // sockaddr_in, htons, htonl, INADDR_ANY
#include <sys/socket.h> // socket(), bind(), listen(), accept(), recv()
#include <unistd.h>     // close()

using namespace std;

int main()
{
    // ===========================
    // 1) Create a listening socket (server socket)
    // ===========================
    // AF_INET + SOCK_STREAM -> TCP over IPv4. Protocol 0 selects IPPROTO_TCP.
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // In real code, check for error:
    // if (serverSocket == -1) { perror("socket"); return 1; }

    // Optional but recommended: allow fast reuse of the port after restart.
    // int opt = 1;
    // setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // ===========================
    // 2) Bind the socket to a local address and port
    // ===========================
    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress)); // clear structure
    serverAddress.sin_family = AF_INET;     // IPv4
    serverAddress.sin_port = htons(8080);   // Port 8080 in network byte order

    // INADDR_ANY means "bind to all local interfaces" (0.0.0.0).
    // This allows clients to connect via any IP assigned to this host.
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind associates the socket with the specified local address:port.
    // After a successful bind, the OS knows where to deliver incoming SYNs.
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // Check in robust code:
    // if (bind(...) == -1) { perror("bind"); close(serverSocket); return 1; }

    // ===========================
    // 3) Mark the socket as passive (listening) for incoming connections
    // ===========================
    // listen(fd, backlog)
    // - backlog is the maximum length for the queue of pending connections
    //   (SYNs that have completed the handshake but not yet accepted).
    // The kernel starts accepting new handshakes on this socket.
    listen(serverSocket, 5);

    // Robust check:
    // if (listen(...) == -1) { perror("listen"); close(serverSocket); return 1; }

    // ===========================
    // 4) Accept a client connection
    // ===========================
    // accept() blocks until a new connection is ready, then returns a NEW socket
    // dedicated to that client. The original serverSocket continues listening
    // for additional clients.
    int clientSocket = accept(serverSocket, nullptr, nullptr);

    // Error check in robust code:
    // if (clientSocket == -1) { perror("accept"); close(serverSocket); return 1; }

    // ===========================
    // 5) Receive data from the client
    // ===========================
    // recv(fd, buffer, capacity, flags) returns number of bytes received,
    // 0 if the peer performed an orderly shutdown (closed), or -1 on error.
    char buffer[1024] = { 0 };
    recv(clientSocket, buffer, sizeof(buffer), 0);

    // For safety, ensure null-termination if you plan to print as C-string:
    // ssize_t n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    // if (n > 0) buffer[n] = '\0';

    cout << "Message from client: " << buffer << endl;

    // ===========================
    // 6) Close sockets
    // ===========================
    // Close the per-client socket first (we're done with that client).
    // Then close the listening socket when the server is shutting down.
    close(clientSocket);
    close(serverSocket);

    return 0;
}