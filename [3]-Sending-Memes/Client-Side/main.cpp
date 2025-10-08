#include <string>
#include <sys/types.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

void sendImagePixelByPixel(int* clientSocket, unsigned char* data, int width, int height, int channels);
void setRGBBackground(int r, int g, int b);
void resetColors();

int main(int argc, char* argv[]) {
    if (argc < 2){
        std::cout << "usage " << argv[0] << " <port> <message sent to server>\n";
        std::cout << "please provide the port to connect!\n";
        return 0;
    }

    std::cout << "Input 0 if you want to send image pixel by pixel\nInput 1 if you want to send image in a single send";
    std::cout << "I will not make this code safe, if you don't input one of that, it will crash\n";
    bool option;
    std::cin >> option;

    std::string path = "image.png";
    int port = std::atoi(argv[1]);

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load image!\n";
        return 1;
    }

    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }
    std::cout << "Width: " << width << "\n";
    std::cout << "Height: " << height << "\n";
    std::cout << "Channels: " << channels << " (3=RGB, 4=RGBA)\n";

    // Setup server address struct
    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // example port
    serverAddress.sin_addr.s_addr = INADDR_ANY; // example address

    int serverConnection = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (serverConnection) { 
        std::cerr << "Something went wrong!\n";
    }

    send(clientSocket, &option, sizeof(option), 0);

    if (option){
        // todo: this shit doesn't work, fix later
        // Send image dimensions first
        std::string imageInfo = std::to_string(width) + "," + std::to_string(height) + "," + std::to_string(channels);
        send(clientSocket, imageInfo.c_str(), imageInfo.size(), 0);
        
        // Send the entire image buffer in one go
        send(clientSocket, data, width * height * channels, 0);
    } else {
        std::string imageInfo = std::to_string(width) + "," + std::to_string(height);
        send(clientSocket, imageInfo.c_str(), imageInfo.size(), 0);

        // Send image pixel by pixel
        sendImagePixelByPixel(&clientSocket, data, width, height, channels);
    }

    close(clientSocket);
    return 0;
}

void setRGBBackground(int r, int g, int b){
    std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m";
}

void resetColors(){
    std::cout << "\033[0m";
}


void sendImageSingleRequest(){

}

void sendImagePixelByPixel(int* clientSocket, unsigned char* data, int width, int height, int channels){
    int sendSpeed = 1000;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * channels;
            unsigned char r = data[index + 0];
            unsigned char g = data[index + 1];
            unsigned char b = data[index + 2];
            //setRGBBackground(r, g, b);
            std::string pixelInfo = std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b);
            //std::cout << "  ";
            // std::string pixel_message = "(" + std::to_string((int)r) + ", " + std::to_string((int)g) + ", " + std::to_string((int)b) + ")";
            // std::cout << pixel_message;

            // Ok, this one is funny
            // By default, std::cout output is buffered, meaning output is stored in memory and only written to the terminal
            // when the buffer is full or a newline is encountered. This can cause multiple pixels (or even a whole row)
            // to be printed at once, rather than one at a time.
            // Calling std::cout.flush() forces the buffer to write its contents to the terminal immediately.
            // This ensures that each pixel is displayed as soon as it is processed, resulting in pixel-by-pixel output
            // instead of row-by-row or chunked output.
            //std::cout.flush();
            send(*clientSocket, pixelInfo.c_str(), pixelInfo.size(), 0);
            usleep(sendSpeed);
        }
        //resetColors(); 
        // std::cout << "\n";
    }
}