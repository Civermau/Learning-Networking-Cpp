#include <cstring>
#include <iostream>

#include <netinet/in.h> // sockaddr_in, htons, htonl, INADDR_ANY
#include <sys/socket.h> // socket(), bind(), listen(), accept(), recv()
#include <unistd.h>     // close()

// class client {
//     public:
//         int socket;
//         std::string message;
// };

int main(int argc, char* argv[])
{
    int port = 7777;

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <port>\n";
        std::cout << "Port not provided, using default port 7777\n";
    } else {
        port = std::atoi(argv[1]);
    }

    //Creating the server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    // This line uses the memset function to set all bytes of the serverAddress structure to zero.
    // This is important because sockaddr_in is a struct that may contain padding or uninitialized fields,
    // and zeroing it out ensures that all fields start with a known value (0), preventing unpredictable behavior.
    // It helps avoid subtle bugs that could arise if the structure contains garbage values from previous stack usage.
    // By passing the address of serverAddress, a value of 0, and the size of the structure, we guarantee that
    // every byte in serverAddress is initialized to zero before we explicitly set the fields we care about.
    std::memset(&serverAddress, 0, sizeof(serverAddress)); // clear structure
    serverAddress.sin_family = AF_INET;     // Define server socket will be using IPv4
    serverAddress.sin_port = htons(port);   // Define the port server socket will be using
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    
    // The bind() function is used here to associate the server socket (serverSocket) with a specific local address and port number.
    // This is a crucial step in setting up a server socket, as it tells the operating system that this socket should listen for incoming
    // connections on the specified port (and on all available network interfaces, since INADDR_ANY is used). The serverAddress structure
    // contains the address family (IPv4), the port (converted to network byte order with htons), and the IP address (INADDR_ANY, which
    // means any local address). By passing the address of this structure, along with its size, to bind(), we ensure that the socket is
    // properly registered with the OS networking stack. If bind() fails (for example, if the port is already in use or if the process
    // lacks the necessary permissions), the server will not be able to accept connections on the desired port. This step must be done
    // before calling listen() or accept(), as it establishes the identity of the server socket for incoming client connections.
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // The listen() function is called here to mark the server socket (serverSocket) as a passive socket,
    // which means it will be used to accept incoming connection requests from clients. This is a crucial
    // step in the lifecycle of a TCP server. After binding the socket to a specific address and port with
    // bind(), the server must call listen() to indicate its readiness to accept connections. The second
    // argument to listen(), in this case 5, specifies the maximum number of pending connections that can
    // be queued up before the system starts rejecting new connection requests. This backlog parameter
    // determines how many clients can wait in line while the server is busy handling other connections.
    // If more than 5 clients attempt to connect simultaneously, additional connection requests may be
    // refused until space becomes available in the queue. Internally, the operating system maintains this
    // queue and manages the transition of connections from the pending state to the established state as
    // the server calls accept() to handle them. Without calling listen(), the server would not be able to
    // accept any incoming connections, and clients attempting to connect would fail. This step is always
    // required for TCP servers that intend to handle incoming client connections.
    listen(serverSocket, 5);
    int clientSocket;

    for (int i = 0; i < 5 ; i++) {
        // The following line calls the accept() function, which is a fundamental part of the server-side socket programming model in TCP/IP networking.
        // When a server socket is set up and listening (after calling listen()), it is ready to accept incoming connection requests from clients.
        // The accept() function is a blocking call, meaning that the program will pause at this line and wait until a client attempts to connect to the server.
        // Once a client initiates a connection, accept() creates a new socket specifically for communicating with that client, while the original server socket
        // (serverSocket) continues to listen for additional incoming connections. This allows the server to handle multiple clients in sequence or, in more
        // advanced designs, concurrently. The first argument to accept() is the listening server socket. The second and third arguments are used to obtain
        // information about the connecting client (such as its address), but in this case, they are set to nullptr, indicating that the server does not need
        // to know or store the client's address information. The return value of accept() is a new socket descriptor (clientSocket) that is used for all
        // subsequent communication with the connected client (such as receiving and sending data). If accept() fails, it returns -1, which should be checked
        // in robust code to handle errors gracefully. In summary, this line is where the server actually establishes a connection with a client and is ready
        // to exchange data with it.
        clientSocket = accept(serverSocket, nullptr, nullptr);
        char buffer[1024] = { 0 };
        ssize_t n = recv(clientSocket, buffer, sizeof(buffer), 0);
        std::cout << "Message from client: " << buffer << std::endl;
    }
    close(clientSocket);
    close(serverSocket);

    return 0;
}
