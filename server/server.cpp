#include "server.hpp"
#include "../include/matrix.hpp"
#include <winsock2.h>
#include <windows.h>

std::atomic<bool> Server::running = true;
Server* Server::instance = nullptr;

bool sendResponse(Socket& client, Matrix& m, double timeSingle, double timeMulti) {
    try {
        const std::string msg = "Parameters received OK!";
        const std::string preview = m.previewMatrix();

        // Create an array of send operations as lambdas
        const std::function<bool()> sendOps[] = {
            //sending msg for parameters
            [&]() { return client.sendInt(msg.size()); },
            [&]() { return client.sendAll(msg.c_str(), msg.size()); },
            //sending times
            [&]() { return client.sendDouble(timeSingle); },
            [&]() { return client.sendDouble(timeMulti); },
            //sending preview
            [&]() { return client.sendInt(preview.size()); },
            [&]() { return client.sendAll(preview.c_str(), preview.size()); }
        };

        // Execute all sends and check success
        for (const auto& op : sendOps) 
            if (!op()) 
                throw std::runtime_error("Failed sending data to client");

        Logger::getInstance().log("Client response sent successfully", LogLevel::INFO);
        return true;
    }
    catch (const std::exception& e) {
        Logger::getInstance().log("Error sending response to client: " + std::string(e.what()), LogLevel::ERR);
        return false;
    }
    catch (...) {
        Logger::getInstance().log("Unknown error occurred during response", LogLevel::ERR);
        return false;
    }
}


void handleClient(Socket client) {
    Logger::getInstance().log("Handling client", LogLevel::INFO);

    try {
        int rows, cols, threads;

        if (!client.receiveInt(rows) ||
            !client.receiveInt(cols) ||
            !client.receiveInt(threads)) 
        {
            Logger::getInstance().log("Failed to receive parameters from client", LogLevel::ERR);
            return;
        }

        if (rows <= 0 || cols <= 0 || threads <= 0) {
            std::string errMsg = "Invalid parameters sent from client!";
            Logger::getInstance().log(errMsg, LogLevel::ERR);
            client.sendInt(errMsg.size());
            client.sendAll(errMsg.c_str(), errMsg.size());
            return;
        }

        Matrix m(rows, cols);

        auto start = std::chrono::high_resolution_clock::now();
        m.fillSingleThreaded();
        auto end = std::chrono::high_resolution_clock::now();
        double timeSingle = std::chrono::duration<double,std::milli>(end - start).count();

        double timeMulti = timeSingle; //same time for when threads=1
        if (threads > 1) {
            start = std::chrono::high_resolution_clock::now();
            m.fillMultiThreaded(threads);
            end = std::chrono::high_resolution_clock::now();
            timeMulti = std::chrono::duration<double,std::milli>(end - start).count();
        }
        else{Logger::getInstance().log("Thread count = 1 -> skipping multithreading", LogLevel::INFO);}

        // Send response safely
        if (!sendResponse(client, m, timeSingle, timeMulti)) {
            Logger::getInstance().log("Failed to send full response to client", LogLevel::ERR);
            return;
        }

        //log success if everything is completed
        Logger::getInstance().log("Client handled successfully", LogLevel::INFO);
    }
    catch (const std::exception& e) {
        Logger::getInstance().log(
            std::string("Exception while handling client: ") + e.what(),
            LogLevel::ERR
        );
    }
    catch (...) {
        Logger::getInstance().log("Unknown exception occurred while handling client", LogLevel::ERR);
    }
}


BOOL WINAPI Server::consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        Logger::getInstance().log("CTRL+C received, shutting down server...", LogLevel::INFO);
        running = false;

        if (instance) {
            closesocket(instance->serverSocket.getFd()); // unblock accept()
        }
        return TRUE;
    }
    return FALSE;
}

Server::Server(int port_, int threadCount)
    : port(port_), pool(threadCount)
{
    instance = this;

    SetConsoleCtrlHandler(consoleHandler, TRUE);

    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    serverSocket = Socket::createTcp();

    if (!serverSocket.isValid())
        throw std::runtime_error("Failed to create server socket");

    if (!serverSocket.bindToPort(port))
        throw std::runtime_error("Failed to bind server to port");

    serverSocket.listenOn();

    Logger::getInstance().log("Server initialized", LogLevel::INFO);
}

Server::~Server() {
    running = false;
    pool.shutdown();         // wait for tasks to finish
    closesocket(serverSocket.getFd());
    WSACleanup();
    Logger::getInstance().log("Server destroyed safely", LogLevel::INFO);
}

void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void Server::printStartupBanner(int port, int threadCount) const{
    setConsoleColor(11); // bright cyan
    std::cout << "====================================\n";
    setConsoleColor(14); // yellow
    std::cout << "      MULTI-THREAD MATRIX SERVER     \n";
    setConsoleColor(11);
    std::cout << "====================================\n";
    setConsoleColor(15); // default white
    std::cout << "Port: " << port << "\n";
    std::cout << "Thread pool size: " << threadCount << "\n";
    std::cout << "Max matrix elements: 100,000,000\n";
    std::cout << "Server starting...\n\n";
}


void Server::run() {
    printStartupBanner(port, pool.getThreadCount());
    
    Logger::getInstance().log("Server waiting for clients...", LogLevel::INFO);

    while (running) {
        Socket client = serverSocket.acceptClient();

        if (!client.isValid()) {
            if (!running) break;
            continue;
        }

        Logger::getInstance().log("Client accepted", LogLevel::INFO);

        pool.enqueue([client = std::make_shared<Socket>(std::move(client))]() {
            handleClient(std::move(*client));
        });
    }

    Logger::getInstance().log("Stopped accepting new clients.", LogLevel::INFO);
}

