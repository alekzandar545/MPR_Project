#include <iostream>
#include "client.hpp"

int main() {
    Client client;

    if (!client.isValid()) {
        std::cerr << "Failed to create client socket\n";
        return 1;
    }

    std::cout << "Connecting to server...\n";
    while (!client.connectToServer("127.0.0.1", 5555)) {
        std::cout << "Connection failed! Retry? (y/n): ";
        char choice;
        std::cin >> choice;
        std::cin.get();
        if (choice != 'y' && choice != 'Y') {
            std::cout << "Exiting client.\n";
            return 1;
        }
        std::cout << "Retrying conncection...\n";
    }
    std::cout << "Connected!\n";

    // Input parameters
    int rows    = getIntFromUser("Enter rows: ");
    int cols    = getIntFromUser("Enter cols: ");
    int threads = getIntFromUser("Enter threads: ");

    if (!client.sendParameters(rows, cols, threads)) {
        std::cerr << "Failed to send parameters to server\n";
        return 1;
    }
    std::cout << "Sent test parameters.\n";

    // Receive response
    std::string message, preview;
    double tSingle, tMulti;
    if (!client.receiveResponse(message, tSingle, tMulti, preview)) {
        std::cerr << "Failed to receive response from server\n";
        system("pause"); //let client read message
        return 1;
    }

    std::cout << "Server says: " << message << "\n\n";
    std::cout << "Single-thread: " << tSingle << " ms\n";
    std::cout << "Multi-thread: " << tMulti << " ms\n\n";
    std::cout << preview << "\n";

    system("pause");
    return 0;
}
