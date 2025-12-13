#pragma once
#include <string>
#include "../include/socket.hpp"

class Client {
private:
    Socket socket_;

public:
    Client();
    ~Client();

    bool connectToServer(const std::string& host, int port);

    //send params to matrix
    bool sendParameters(int rows, int cols, int threads);

    //receive server response (message,timings,preview)
    bool receiveResponse(std::string& message, double& timeSingle, double& timeMulti, std::string& preview);

    //valid socket
    bool isValid() const;
};

//for safe integer input
int getIntFromUser(const std::string& prompt);
