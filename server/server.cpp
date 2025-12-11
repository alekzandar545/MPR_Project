#include "server.hpp"
#include "../include/matrix.hpp"
#include <windows.h>
#include <winsock2.h>

std::atomic<bool> Server::running = true;
Server* Server::instance = nullptr;

void handleClient(Socket client) {
    Logger::getInstance().log("Handling client", LogLevel::INFO);

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

    if (rows <= 0 || cols <= 0 || threads <= 0) {
        std::string errMsg = "Invalid parameters!";
        client.sendInt(errMsg.size());
        client.sendAll(errMsg.c_str(), errMsg.size());
        return;
    }

    Matrix m(rows, cols);

    auto start = std::chrono::high_resolution_clock::now();
    m.fillSingleThreaded();
    auto end = std::chrono::high_resolution_clock::now();
    double timeSingle = std::chrono::duration<double,std::milli>(end - start).count();

    double timeMulti = timeSingle;
    if (threads > 1) {
        start = std::chrono::high_resolution_clock::now();
        m.fillMultiThreaded(threads);
        end = std::chrono::high_resolution_clock::now();
        timeMulti = std::chrono::duration<double,std::milli>(end - start).count();
    }

    std::string msg = "Parameters received OK!";
    client.sendInt(msg.size());
    client.sendAll(msg.c_str(), msg.size());

    client.sendDouble(timeSingle);
    client.sendDouble(timeMulti);

    std::string preview = m.previewMatrix();
    client.sendInt(preview.size());
    client.sendAll(preview.c_str(), preview.size());
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

void Server::run() {
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

