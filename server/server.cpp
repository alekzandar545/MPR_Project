#include <iostream>
#include "../include/socket.hpp"
#include <thread>
#include <chrono>
#include "../include/matrix.hpp"

void handleClient(Socket& client) {
    std::cout << "Handling client in thread " 
              << std::this_thread::get_id() << "\n";

    int rows, cols, threads;

    client.receiveInt(rows);
    client.receiveInt(cols);
    client.receiveInt(threads);

    std::cout << "Received params:"
            << " rows=" << rows
            << " cols=" << cols
            << " threads=" << threads << "\n";


    Matrix m(rows, cols);

    auto start = std::chrono::high_resolution_clock::now();
    m.fillSingleThreaded([](int r,int c){ return (r+c)%17; });
    auto end = std::chrono::high_resolution_clock::now();
    double timeSingle = std::chrono::duration<double,std::milli>(end-start).count();

    start = std::chrono::high_resolution_clock::now();
    m.fillMultiThreaded([](int r,int c){ return (r+c)%17; }, threads);
    end = std::chrono::high_resolution_clock::now();
    double timeMulti = std::chrono::duration<double,std::milli>(end-start).count();

        
    //response
    std::string msg = "Parameters received OK!";
    int len = msg.size();
    client.sendInt(len);
    client.sendRaw(msg.c_str(), len);

    client.sendDouble(timeSingle);
    client.sendDouble(timeMulti);

    std::cout << "Client disconnected\n";
}


int main() {
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    Socket server = Socket::createTcp();
    if (!server.isValid()) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    if (!server.bindToPort(5555)) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    server.listenOn();

    std::cout << "Server waiting for client...\n";


    while (true) {
        Socket client = server.acceptClient();
        if (!client.isValid())
            continue;

        std::cout << "Client connected!\n";

        std::thread(
            [client = std::move(client)]() mutable 
            {
                handleClient(client);
            }
        ).detach();
    }

    WSACleanup();
    return 0;
}
