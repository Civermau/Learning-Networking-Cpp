#include <cstring>
#include <iostream>
#include <netinet/in.h> // sockaddr_in, htons, etc.
#include <arpa/inet.h>  // inet_pton, inet_ntop
#include <string>
#include <sys/socket.h> // socket(), connect(), send(), etc.
#include <sys/types.h>
#include <unistd.h>     // close()


class Client{
    std::string name;
    std::pair<int, int> position;
    std::string chatBuffer;
    int clientSocket;
    sockaddr_in serverAddress;
    int port;

public:
  Client(int port = 7777, int x = 0, int y = 0, const std::string& clientName = "", const std::string& serverIp = "127.0.0.1"){
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    name = clientName;
    position.first = x;
    position.second = y;  
    this->port = port; 
    
    std::memset(&serverAddress, 0, sizeof(serverAddress)); // zero it for safety
    serverAddress.sin_family = AF_INET; // Client socket will be using IPv4 protocol
    serverAddress.sin_port = htons(port); // Use provided port

    // Convert string IP address to network order, error if fails
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cout << "Invalid server IP address: " << serverIp << std::endl;
        return;
    }

    char readableIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &serverAddress.sin_addr, readableIp, INET_ADDRSTRLEN);

    std::cout << "Connecting to " << readableIp << " at port " << port << std::endl;
    int err = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    if (err){
        std::cout << "Something went wrong! Error: " << strerror(errno) << std::endl;
    }   
  }

  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;
}; 