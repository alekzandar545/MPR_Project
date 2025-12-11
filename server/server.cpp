#include <thread>
#include <chrono>
#include "../include/socket.hpp"
#include "../include/matrix.hpp"
#include "../include/thread_pool.hpp"
#include "../include/logger.hpp"   

void handleClient(Socket client) {
    Logger::getInstance().log(
        "Handling client", LogLevel::INFO
    );

    //receiving data from client
    int rows, cols, threads;

    if (!client.receiveInt(rows) ||
        !client.receiveInt(cols) ||
        !client.receiveInt(threads)) 
    {
        Logger::getInstance().log(
            "Failed to receive parameters from client",
            LogLevel::ERR
        );
        return;
    }

    Logger::getInstance().log(
        "Received params: rows=" + std::to_string(rows) +
        " cols=" + std::to_string(cols) +
        " threads=" + std::to_string(threads),
        LogLevel::DEBUG
    );

    //processing the data from the client
    Matrix m(rows, cols);

    auto start = std::chrono::high_resolution_clock::now();
    m.fillSingleThreaded();
    auto end = std::chrono::high_resolution_clock::now();
    double timeSingle = std::chrono::duration<double,std::milli>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    m.fillMultiThreaded(threads);
    end = std::chrono::high_resolution_clock::now();
    double timeMulti = std::chrono::duration<double,std::milli>(end - start).count();

    //sending response
    std::string msg = "Parameters received OK!";
    int len = msg.size();
    client.sendInt(len);
    client.sendRaw(msg.c_str(), len);

    //sending times
    client.sendDouble(timeSingle);
    client.sendDouble(timeMulti);

    //sending preview
    std::string preview = m.previewMatrix();
    len = preview.size();

    client.sendInt(len);
    client.sendAll(preview.c_str(), preview.size());

    Logger::getInstance().log("Client disconnected", LogLevel::INFO);
}


int main() {
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    Socket server = Socket::createTcp();
    if (!server.isValid()) {
        Logger::getInstance().log("Socket creation failed", LogLevel::ERR);
        return 1;
    }

    if (!server.bindToPort(5555)) {
        Logger::getInstance().log("Bind failed", LogLevel::ERR);
        return 1;
    }

    server.listenOn();

    Logger::getInstance().log("Server waiting for client...", LogLevel::INFO);

    ThreadPool pool(8); // depends on CPU cores

    while (true) {
        Socket client = server.acceptClient();
        if (!client.isValid()) continue;

        Logger::getInstance().log("Client accepted successfully!", LogLevel::INFO);

        pool.enqueue([clientPtr = std::make_shared<Socket>(std::move(client))]() mutable {
            handleClient(std::move(*clientPtr));
        });
    }

    WSACleanup();
    return 0;
}
