#include <iostream>
#include "socket.hpp"

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
    int rows = 10;
    int cols = 10;
    int threads = 4;

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

    std::cout << "Single-thread: " << tSingle << " ms\n";
    std::cout << "Multi-thread: " << tMulti << " ms\n";

    WSACleanup();
    return 0;
}
