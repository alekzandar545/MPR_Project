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
    int rows, cols, threads;
    std::cout << "Enter rows: ";
    std::cin >> rows;
    std::cout << "Enter cols: ";
    std::cin >> cols;
    std::cout << "Enter threads: ";
    std::cin >> threads;

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
