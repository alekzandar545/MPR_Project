#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // needed for Windows builds

class Socket {
public:
    //life cycle
    Socket();
    explicit Socket(SOCKET fd);
    static Socket createTcp(); //static constructor
    ~Socket();
  
    //getters
    SOCKET getFd() const;
    
    //removed copying
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    
    //instead only move operations
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    //server-side
    bool bindToPort(int port);
    bool listenOn(int backlog = SOMAXCONN);
    Socket acceptClient();

    //client-side
    bool connectTo(const std::string& ip, int port);

    //data operations
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
