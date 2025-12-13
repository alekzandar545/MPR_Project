#include "client.hpp"
#include <iostream>

Client::Client() {
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    socket_ = Socket::createTcp();
}

Client::~Client() {
    //socket RAII will handle closing
    WSACleanup();
}

bool Client::connectToServer(const std::string& host, int port) {
    return socket_.connectTo(host, port);
}

bool Client::sendParameters(int rows, int cols, int threads) {
    if (!socket_.isValid()) return false;
    return socket_.sendInt(rows) &&
           socket_.sendInt(cols) &&
           socket_.sendInt(threads);
}

bool Client::receiveResponse(std::string& message, double& timeSingle, double& timeMulti, std::string& preview) {
    if (!socket_.isValid()) return false;

    int len;
    if (!socket_.receiveInt(len)) return false;

    message.resize(len);
    if (!socket_.receiveRaw(message.data(), len)) return false;

    if (!socket_.receiveDouble(timeSingle) || !socket_.receiveDouble(timeMulti)) return false;

    if (!socket_.receiveInt(len)) return false;
    preview.resize(len);
    if (!socket_.receiveRaw(preview.data(), len)) return false;

    return true;
}

bool Client::isValid() const {
    return socket_.isValid();
}

//safe integer input
int getIntFromUser(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt; 
        std::string input;
        std::getline(std::cin, input);

        try {
            size_t pos;
            value = std::stoi(input, &pos);
            if (pos != input.size()) throw std::invalid_argument("Extra characters after number");
            break;
        } catch (std::exception&) {
            std::cout << "Invalid input. Please enter a valid integer.\n";
        }
    }
    return value;
}
