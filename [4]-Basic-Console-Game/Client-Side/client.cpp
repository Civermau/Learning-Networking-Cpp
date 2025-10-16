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
  Client(int port = 7777, const std::string& serverIp = "127.0.0.1", const std::string& clientName = ""){
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    name = clientName;
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

  // These lines delete the copy constructor and assignment operator,
  // which means you cannot make copies of a Client instance. This is
  // often done to ensure that each Client is unique (has "unique ownership"),
  // especially when it is managing resources like sockets or connections
  // that cannot be safely duplicated.
  //
  // For example, both of these will cause a compile error:
  //   Client c1(1234);
  //   Client c2 = c1;      // ERROR: can't copy-construct
  //   Client c3(5678);
  //   c3 = c1;             // ERROR: can't assign
  //
  // Also, if you try to pass a Client object to a function by value,
  // it will try to make a copy, and will also fail:
  //   void doSomething(Client c);   // ERROR!
  //   doSomething(c1);              // ERROR: copy is deleted
  // So you should always use a reference or pointer instead:
  //   void doSomething(Client& c);      // ok
  //   void doSomething(Client* c);      // ok
  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;
}; 