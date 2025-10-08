#include <cstddef>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

void splitRGB(char buffer[], int *r, int *g, int *b);
void setRGBBackground(int r, int g, int b);
void resetColors();

struct Pixel {
  int r;
  int g;
  int b;
  Pixel(int red = 0, int green = 0, int blue = 0) : r(red), g(green), b(blue) {}
};

// Unsorted list of pixels
std::vector<Pixel> pixelList;

int main(int argc, char *argv[]) {
  int port = 7777;

  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <port>\n";
    std::cout << "Port not provided, using default port 7777\n";
  } else {
    port = std::atoi(argv[1]);
  }

  // Create a socket
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    std::cerr << "Failed to create socket\n";
    return 1;
  }

  // Setup server address struct
  sockaddr_in serverAddress;
  std::memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);       // example port
  serverAddress.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces

  int binding = bind(serverSocket, (struct sockaddr *)&serverAddress,
                     sizeof(serverAddress));
  if (binding) {
    std::cerr << "Something went wrong!\n";
  }

  listen(serverSocket, 5);

  int clientSocket = accept(serverSocket, nullptr, nullptr);
  char buffer[1024] = {0};

  bool option;
  // In the first recv, we pass &option (a pointer to option) as the buffer argument,
  // so that the received data is written directly into the 'option' variable.
  // This is because we want to receive a single boolean value from the client and store it in 'option'.
  // In contrast, in the second recv, we pass 'buffer' (an array) as the buffer argument,
  // so that the received data is written into the buffer array for further processing as a string or data block.
  // In both cases, recv expects a pointer to the memory where the received data should be stored,
  // but the type of the variable (bool vs. char array) determines what we pass.
  ssize_t n = recv(clientSocket, &option, sizeof(option), 0);

  if (option) {
    std::cout << "Client chose to send the image in a single send (option=1)" << std::endl;
    
    // First, receive image dimensions
    char dimBuffer[64];
    n = recv(clientSocket, dimBuffer, sizeof(dimBuffer), 0);
    if (n <= 0) {
        std::cerr << "Failed to receive image dimensions\n";
        return 1;
    }
    dimBuffer[n] = '\0';
    
    // Parse dimensions
    std::string dimInfo(dimBuffer);
    size_t comma1 = dimInfo.find(',');
    size_t comma2 = dimInfo.find(',', comma1 + 1);
    
    int width = std::stoi(dimInfo.substr(0, comma1));
    int height = std::stoi(dimInfo.substr(comma1 + 1, comma2 - comma1 - 1));
    int channels = std::stoi(dimInfo.substr(comma2 + 1));
    
    std::cout << "Image dimensions: " << width << "x" << height << "x" << channels << std::endl;
    
    // Allocate memory for image data
    unsigned char* data = new unsigned char[width * height * channels];
    
    // Receive the actual image data
    size_t totalBytes = width * height * channels;
    size_t receivedBytes = 0;
    
    while (receivedBytes < totalBytes) {
        n = recv(clientSocket, data + receivedBytes, totalBytes - receivedBytes, 0);
        if (n <= 0) {
            std::cerr << "Failed to receive image data\n";
            delete[] data;
            return 1;
        }
        receivedBytes += n;
    }
    
    std::cout << "Successfully received " << receivedBytes << " bytes of image data\n";
    
    // Process or display the image data here
    // ... your image processing code ...
    
    delete[] data;
  } else {
    std::cout << "Client chose to send the image pixel by pixel (option=0)" << std::endl;

    n = recv(clientSocket, buffer, sizeof(buffer), 0);

    int width, height;
    // Split buffer using ',' as delimiter
    std::string imageInfo(buffer, n);
    size_t commaPos = imageInfo.find(',');
    if (commaPos != std::string::npos) {
      width = std::stoi(imageInfo.substr(0, commaPos));
      height = std::stoi(imageInfo.substr(commaPos + 1));
    } else {
      std::cerr << "Invalid image info received!\n";
      width = height = 0;
    }

    printf("Width: %d\n", width);
    printf("Height: %d\n", height);
    int r, g, b;

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        n = recv(clientSocket, buffer, sizeof(buffer), 0);
        // Add a null terminator to the received buffer so that it can be safely
        // used as a C-string. Without this, when parsing the RGB values,
        // leftover data from previous messages or garbage memory could be
        // interpreted, leading to incorrect color values (e.g., black pixels
        // showing up as blue). This ensures that only the current received
        // message is processed.
        buffer[n] = '\0';
        splitRGB(buffer, &r, &g, &b);
        pixelList.push_back({r, g, b});
      }
    }
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          Pixel currentPixel = pixelList[y * width + x];
          setRGBBackground(currentPixel.r, currentPixel.g, currentPixel.b);
          std::cout << "  ";
          std::cout.flush();
        }
        resetColors();
        std::cout << "\n";
      }
  }

  close(serverSocket);
  close(clientSocket);
  return 0;
}

void setRGBBackground(int r, int g, int b) {
  std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m";
}

void resetColors() { std::cout << "\033[0m"; }

void splitRGB(char buffer[], int *r, int *g, int *b) {
  std::string RGBInfo(buffer);
  size_t firstComma = RGBInfo.find(',');
  size_t secondComma = RGBInfo.find(',', firstComma + 1);

  if (firstComma != std::string::npos && secondComma != std::string::npos) {
    *r = std::stoi(RGBInfo.substr(0, firstComma));
    *g =
        std::stoi(RGBInfo.substr(firstComma + 1, secondComma - firstComma - 1));
    *b = std::stoi(RGBInfo.substr(secondComma + 1));
  } else {
    *r = *g = *b = 0; // fallback in case of bad format
  }
  return;
}