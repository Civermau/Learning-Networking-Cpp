#include <cstring>
#include <iostream>

#include <netinet/in.h> // sockaddr_in, htons, htonl, INADDR_ANY
#include <sys/socket.h> // socket(), bind(), listen(), accept(), recv()
#include <unistd.h>     // close()

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 1) {
        cout << "Usage: " << argv[0] << " <port>" << endl;
        cout << "Port not provided, using default port 7777" << endl;
    }

    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress)); // clear structure
    serverAddress.sin_family = AF_INET;     // IPv4
    serverAddress.sin_port = htons(7777);   // Port 8080 in network byte order
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    listen(serverSocket, 5);

    int clientSocket = accept(serverSocket, nullptr, nullptr);

    char buffer[1024] = { 0 };
    recv(clientSocket, buffer, sizeof(buffer), 0);

    cout << "Message from client: " << buffer << endl;

    close(clientSocket);
    close(serverSocket);

    return 0;
}