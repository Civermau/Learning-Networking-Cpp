#include <cstring>
#include <netinet/in.h> // sockaddr_in, htons, htonl, INADDR_ANY
#include <sys/socket.h> // socket(), bind(), listen(), accept(), recv()
#include <thread>
#include <unistd.h> // close()

#include <iostream>
#include <string>
#include <vector>

#include "../ui.h"

// * ----------------------------------- Client Socket class for managing the clients

class ClientSocket {
public:
  // Maybe use socket ID?
  int id;
  std::string name;
  std::pair<int, int> position;
  std::string chatBuffer;
  int clientSocket;

  ClientSocket(int clientId, int x = 0, int y = 0,
               const std::string &clientName = "")
      : id(clientId), name(clientName), position(x, y), chatBuffer("") {}

public:
  void setPosition(int x, int y) {
    position.first = x;
    position.second = y;
  }

  void asyncReceiveMessages() {
    std::thread([this]() {
      char buffer[4096];
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }).detach();
  }

  void appendToChatBuffer(const std::string &msg) { chatBuffer += msg; }
};

// * ------------------------------------- Serveer class

class Server {
  int port;
  int serverSocket;
  sockaddr_in serverAddress;
  std::vector<ClientSocket> clientList;

public:
  Server(int port = 7777) {
    clearScreen();
    this->port = port;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
      std::cout << "socket() failed: " << strerror(errno) << std::endl;
      return;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0) {
      std::cout << "bind() failed on port " << port << ": " << strerror(errno) << std::endl;
      close(serverSocket);
      return;
    }

    if (listen(serverSocket, 5) != 0) {
      std::cout << "listen() failed: " << strerror(errno) << std::endl;
      close(serverSocket);
      return;
    }
  }
  // Example async methods using std::thread (simulate async in a simple way)

  // * ------------------------------------------------- Accept Clients Async
  void asyncAcceptClients() {
    std::thread([this]() {
      while (true) {
        // std::cout << "Listening for clients...\n";
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSock =
            accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSock > 0) {
          ClientSocket newClient(clientList.size() + 1);
          newClient.clientSocket = clientSock;
          newClient.name = "User" + std::to_string(newClient.id);
          newClient.position = {0, 0};
          clientList.push_back(newClient);

          std::cout << "\nNew client connected! (id: " << newClient.id << ")\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
    }).detach();
  }


// * ------------------------------------------------- Broadcaste messages Async
  void asyncBroadcastMessages() {
    std::thread([this]() {
      while (true) {
        // std::cout << "trying to broadcast";
        for (auto &client : clientList) {
          if (!client.chatBuffer.empty()) {
            // Broadcast to all clients except the sender
            for (auto &target : clientList) {
              if (target.clientSocket != client.clientSocket) {
                send(target.clientSocket, client.chatBuffer.c_str(),
                     client.chatBuffer.size(), 0);
              }
            }
            client.chatBuffer.clear();
          }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
      }
    }).detach();
  }

  // Call asyncs in start()
  void start() {
    //clearScreen();
    std::cout << "now listening for new clients";
    std::cout.flush();

    asyncAcceptClients();
    asyncBroadcastMessages();

    for (;;) {
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
  }

  ~Server() {
    for (int i = 0; i < clientList.size(); i++) {
      close(clientList[i].clientSocket);
    }
    close(serverSocket);
  }

  // These lines delete the copy constructor and assignment operator,
  // which means you cannot make copies of a Server instance. This is
  // often done to ensure that each Server is unique (has "unique ownership"),
  // especially when it is managing resources like sockets or threads
  // that cannot be safely duplicated.
  //
  // For example, both of these will cause a compile error:
  //   Server s1(1234);
  //   Server s2 = s1;      // ERROR: can't copy-construct
  //   Server s3(5678);
  //   s3 = s1;             // ERROR: can't assign
  //
  // Also, if you try to pass a Server object to a function by value,
  // it will try to make a copy, and will also fail:
  //   void doSomething(Server s);   // ERROR!
  //   doSomething(s1);              // ERROR: copy is deleted
  // So you should always use a reference or pointer instead:
  //   void doSomething(Server& s);      // ok
  //   void doSomething(Server* s);      // ok
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;
};
