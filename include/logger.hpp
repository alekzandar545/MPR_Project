#pragma once
#include <fstream>
#include <mutex>
#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <thread>
#include <map>

enum class LogLevel { INFO, DEBUG, WARN, ERR };

class Logger {
public:
    static Logger& getInstance();  // singleton

    void log(const std::string& message, LogLevel level = LogLevel::INFO);

    // optional: change log file at runtime
    void setLogFile(const std::string& filename);

private:
    Logger();  // private for singleton
    ~Logger();
    std::ofstream logfile_;
    std::mutex mtx_;

    std::string levelToString(LogLevel level);
    std::string timestamp();

    int getThreadId();

    std::mutex threadIdMutex_;
    std::map<std::thread::id, int> threadIdMap_;
    int nextThreadId_ = 0;
};
