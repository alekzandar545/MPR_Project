#include "server.hpp"

int main() {
    try {
        Server server(5555, 8);
        server.run();
    } 
    catch (const std::exception& e) {
        Logger::getInstance().log(std::string("Fatal error: ") + e.what(), LogLevel::ERR);
    }
}
