#include "../include/socket.hpp"

Socket::Socket() : m_socket(INVALID_SOCKET) {}

Socket::Socket(SOCKET fd) : m_socket(fd) {}

Socket::~Socket() {
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
    }
}

Socket::Socket(Socket&& other) noexcept : m_socket(other.m_socket) {
    other.m_socket = INVALID_SOCKET;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        if (m_socket != INVALID_SOCKET)
            closesocket(m_socket);
        m_socket = other.m_socket;
        other.m_socket = INVALID_SOCKET;
    }
    return *this;
}

Socket Socket::createTcp() {
    SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return Socket(fd);
}

bool Socket::bindToPort(int port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    return bind(m_socket, (sockaddr*)&addr, sizeof(addr)) != SOCKET_ERROR;
}

bool Socket::listenOn(int backlog) {
    return listen(m_socket, backlog) != SOCKET_ERROR;
}

Socket Socket::acceptClient() {
    SOCKET clientFd = accept(m_socket, nullptr, nullptr);
    return Socket(clientFd);
}

bool Socket::connectTo(const std::string& ip, int port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
    return connect(m_socket, (sockaddr*)&addr, sizeof(addr)) != SOCKET_ERROR;
}

int Socket::sendAll(const char* data, int length) {
    return send(m_socket, data, length, 0);
}

int Socket::receive(char* buffer, int length) {
    return recv(m_socket, buffer, length, 0);
}

bool Socket::isValid() const {
    return m_socket != INVALID_SOCKET;
}

bool Socket::sendRaw(const char* data, int len) {
    int sent = 0;
    while (sent < len) {
        int result = send(m_socket, data + sent, len - sent, 0);
        if (result <= 0) return false;
        sent += result;
    }
    return true;
}

bool Socket::receiveRaw(char* data, int len) {
    int received = 0;
    while (received < len) {
        int result = recv(m_socket, data + received, len - received, 0);
        if (result <= 0) return false;
        received += result;
    }
    return true;
}

bool Socket::sendInt(int value) {
    return sendRaw(reinterpret_cast<char*>(&value), sizeof(int));
}

bool Socket::receiveInt(int& value) {
    return receiveRaw(reinterpret_cast<char*>(&value), sizeof(int));
    // if (bytesRead != sizeof(value)) { //doesnt accept anything longr
    //     return false;
    // }
    // return true;
}

bool Socket::sendDouble(double value) {
    return sendRaw(reinterpret_cast<char*>(&value), sizeof(double));
}

bool Socket::receiveDouble(double& value) {
    return receiveRaw(reinterpret_cast<char*>(&value), sizeof(double));
    //     if (bytesRead != sizeof(value)) { 
    //     return false;
    // }
    // return true;
}

SOCKET Socket::getFd() const{
    return m_socket;
}