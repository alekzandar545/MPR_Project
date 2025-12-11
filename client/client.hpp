#pragma once
#include <string>
#include "../include/socket.hpp"

class Client {
private:
    Socket socket_;

public:
    Client();
    ~Client();

    // Connect to server
    bool connectToServer(const std::string& host, int port);

    // Send matrix parameters
    bool sendParameters(int rows, int cols, int threads);

    // Receive server response (message + timings + preview)
    bool receiveResponse(std::string& message, double& timeSingle, double& timeMulti, std::string& preview);

    // Check if socket is valid
    bool isValid() const;
};

// Helper for safe integer input
int getIntFromUser(const std::string& prompt);
