#include "../include/logger.hpp"

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    logfile_.open("logs/server.log", std::ios::app);
}

Logger::~Logger() {
    if (logfile_.is_open()) logfile_.close();
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (logfile_.is_open()) logfile_.close();
    logfile_.open(filename, std::ios::app);
}

std::string Logger::levelToString(LogLevel level) {
    switch(level) {
        case LogLevel::INFO:  return "INFO";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERR: return "ERROR";
    }
    return "UNKNOWN";
}

std::string Logger::timestamp() {
    auto now = std::chrono::system_clock::now();
    auto itt = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &itt);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Logger::log(const std::string& message, LogLevel level) {
    std::lock_guard<std::mutex> lock(mtx_);
    std::string logMessage = "[" + timestamp() + "] [" + levelToString(level) + "] "
                             + "[Thread-" + std::to_string(getThreadId()) + "] "
                             + message;

    std::cout << logMessage << std::endl;
    if (logfile_.is_open())
        logfile_ << logMessage << std::endl;
    
}

int Logger::getThreadId(){
    std::lock_guard<std::mutex> lock(threadIdMutex_);
    auto tid = std::this_thread::get_id();

    if (threadIdMap_.count(tid) == 0) {
        threadIdMap_[tid] = nextThreadId_++;
    }
    return threadIdMap_[tid];
}