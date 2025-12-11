#include <thread>
#include <chrono>
#include "../include/socket.hpp"
#include "../include/matrix.hpp"
#include "../include/thread_pool.hpp"
#include "../include/logger.hpp"   

bool sendResponse(Socket& client, Matrix& m, double timeSingle, double timeMulti){
    try{
        //sending response
        std::string msg = "Parameters received OK!";
        int msgLen = msg.size();
        client.sendInt(msgLen);
        client.sendAll(msg.c_str(), msgLen);

        //sending times
        client.sendDouble(timeSingle);
        client.sendDouble(timeMulti);

        //sending preview
        std::string preview = m.previewMatrix();
        int previewLen = preview.size();

        client.sendInt(previewLen);
        client.sendAll(preview.c_str(), preview.size());

        Logger::getInstance().log("Client disconnected", LogLevel::INFO);
        return true;
    }
    catch (const std::exception& e) {
        Logger::getInstance().log(
            std::string("Error sending response to client: ") + e.what(),
            LogLevel::ERR
        );
        return false;
    }
    catch (...) {
        Logger::getInstance().log("Unknown error occurred during response", LogLevel::ERR);
        return false;
    }
}

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

    //sanity check
    if (rows <= 0 || cols <= 0 || threads <= 0) {
        Logger::getInstance().log(
            "Received invalid parameters: rows=" + std::to_string(rows) +
            " cols=" + std::to_string(cols) +
            " threads=" + std::to_string(threads),
            LogLevel::ERR
        );
        std::string errMsg = "Invalid parameters!";
        try {
            client.sendInt(errMsg.size());
            client.sendAll(errMsg.c_str(), errMsg.size());
        } catch (...) {
            Logger::getInstance().log("Failed to send error message to client", LogLevel::ERR);
        }
        return;
    }
    //limiting the size a bit
    const size_t MAX_ELEMENTS = 100'000'000; // ~400MB for int
    if (static_cast<size_t>(rows) * cols > MAX_ELEMENTS) {
        Logger::getInstance().log("Matrix too large, rejecting request", LogLevel::ERR);

        std::string errMsg = "Matrix too large!";
        try {
            client.sendInt(errMsg.size());
            client.sendAll(errMsg.c_str(), errMsg.size());
        } catch (...) {
            Logger::getInstance().log("Failed to send error message to client", LogLevel::ERR);
        }
        return;
    }

    //everything is okay
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

    double timeMulti = timeSingle; //in case thread = 1
    if (threads > 1) { 
        start = std::chrono::high_resolution_clock::now();
        m.fillMultiThreaded(threads);
        end = std::chrono::high_resolution_clock::now();
        timeMulti = std::chrono::duration<double,std::milli>(end - start).count();
    }
    sendResponse(client,m,timeSingle,timeMulti);
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
