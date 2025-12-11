#pragma once
#include <atomic>
#include "../include/socket.hpp"
#include "../include/thread_pool.hpp"
#include "../include/logger.hpp"

class Server {
public:
    Server(int port, int threadCount);
    ~Server();

    void run();

private:
    static BOOL WINAPI consoleHandler(DWORD signal);
    void printStartupBanner(int port, int threadCount) const;
    
private:
    int port;
    ThreadPool pool;
    Socket serverSocket;

    static std::atomic<bool> running;
    static Server* instance; // for triggering shutdown
};
