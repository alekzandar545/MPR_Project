#include <iostream>
#include "socket.hpp"

//assuring that input for rows/cols/threads is correct and server doesnt crash
int getIntFromUser(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);

        try {
            size_t pos;
            value = std::stoi(input, &pos);
            if (pos != input.size()) {
                throw std::invalid_argument("Extra characters after number");
            }
            break; // valid integer
        } catch (std::exception&) {
            std::cout << "Invalid input. Please enter a valid integer.\n";
        }
    }
    return value;
}

int main() {
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    Socket client = Socket::createTcp();

    if (!client.isValid()) {
        std::cerr << "Failed to create client socket\n";
        return 1;
    }

    std::cout << "Connecting to server...\n";

    if (!client.connectTo("127.0.0.1", 5555)) {
        std::cerr << "Connection failed!\n";
        return 1;
    }

    std::cout << "Connected!\n";

    // TEST: send parameters
    int rows   = getIntFromUser("Enter rows: ");
    int cols   = getIntFromUser("Enter cols: ");
    int threads = getIntFromUser("Enter threads: ");

    client.sendInt(rows);
    client.sendInt(cols);
    client.sendInt(threads);

    std::cout << "Sent test parameters.\n";

    // TEST: receive string length + string
    int len;
    client.receiveInt(len);

    std::string response(len, 0);
    client.receiveRaw(response.data(), len);

    std::cout << "Server says: " << response << "\n";

    double tSingle, tMulti;
    client.receiveDouble(tSingle);
    client.receiveDouble(tMulti);

    std::cout << '\n';
    std::cout << "Single-thread: " << tSingle << " ms\n";
    std::cout << "Multi-thread: " << tMulti << " ms\n";

    //previewing the matrix
    std::cout << '\n';
    client.receiveInt(len);
    std::string preview(len,0);
    client.receiveRaw(preview.data(), len);
    std::cout << preview;

    std::cout << '\n';
    system("pause"); //review results

    WSACleanup();
    return 0;
}
