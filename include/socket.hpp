#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // needed for Windows builds

class Socket {
public:
    // Constructors
    Socket();
    explicit Socket(SOCKET fd);
  
    // Getter
    SOCKET getFd() const;

    // Move-only (no copy)
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    // No copying
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    ~Socket();

    // Static constructors
    static Socket createTcp();

    // Server-side operations
    bool bindToPort(int port);
    bool listenOn(int backlog = SOMAXCONN);
    Socket acceptClient();

    // Client-side operations
    bool connectTo(const std::string& ip, int port);

    // Data operations
    int sendAll(const char* data, int length);
    int receive(char* buffer, int length);

    bool isValid() const;

    //helpful functions
    bool sendInt(int value);
    bool receiveInt(int& value);
    bool sendDouble(double value);
    bool receiveDouble(double& value);
    bool sendRaw(const char* data, int len);
    bool receiveRaw(char* data, int len);

private:
    SOCKET m_socket;
};
