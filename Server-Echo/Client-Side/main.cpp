#include <cstring>
#include <iostream>

#include <netinet/in.h> // sockaddr_in, htons, etc.
#include <ostream>
#include <sys/socket.h> // socket(), connect(), send(), etc.
#include <sys/types.h>
#include <unistd.h>     // close()

int main(int argc, char* argv[])
{
    if (argc < 2){
        std::cout << "usage " << argv[0] << " <port> <message sent to server>\n";
        std::cout << "please provide the port to connect!\n";
        return 0;
    }

    //Creating the client socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    // This line uses the memset function to set all bytes of the serverAddress structure to zero.
    // This is important because sockaddr_in is a struct that may contain padding or uninitialized fields,
    // and zeroing it out ensures that all fields start with a known value (0), preventing unpredictable behavior.
    // It helps avoid subtle bugs that could arise if the structure contains garbage values from previous stack usage.
    // By passing the address of serverAddress, a value of 0, and the size of the structure, we guarantee that
    // every byte in serverAddress is initialized to zero before we explicitly set the fields we care about.
    std::memset(&serverAddress, 0, sizeof(serverAddress)); // zero it for safety

    int port = std::atoi(argv[1]);

    serverAddress.sin_family = AF_INET; // Client socket will be using IPv4 protocol
    serverAddress.sin_port = htons(port); // Port that will be trying to connect client
    serverAddress.sin_addr.s_addr = INADDR_ANY; 

    // The connect() function is used here to establish a connection from the client socket (clientSocket)
    // to the server specified by the serverAddress structure. The function attempts to make a connection
    // to the server's IP address and port. The return value of connect() is 0 on success, indicating that
    // the connection was successfully established. If connect() fails, it returns -1, and the specific
    // error code can be retrieved from errno. Common reasons for failure include: the server is not running,
    // the address is incorrect, the port is not open, or there are network issues.
    int err = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    if (err){
        std::cout << "Something went wrong! Error: " << strerror(errno) << std::endl;
    }

    const char* message = "Hello, server!";
    if (argc > 2) {
        message = argv[2];
    }

    send(clientSocket, message, std::strlen(message), 0);
    char buffer[1024] = { 0 };
    recv(clientSocket, buffer, sizeof(buffer), 0);

    std::cout << "Echo from server: " << buffer << std::endl;

    close(clientSocket);

    return 0;
}
