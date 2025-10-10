#include <vector>
#include <string>

class Client {
public:
    // Maybe use socket ID?
    int id;
    std::string name;
    std::pair<int, int> position;
    std::string chatBuffer;

    Client(int clientId, int x = 0, int y = 0, const std::string& clientName = "")
        : id(clientId), name(clientName), position(x, y), chatBuffer("") {}

    void setPosition(int x, int y) {
        position.first = x;
        position.second = y;
    }

    void appendToChatBuffer(const std::string& msg) {
        chatBuffer += msg;
    }
};

using ClientsList = std::vector<Client>;


int server() {
    return 0;
}


