#include <iostream>

#include "log.h"

namespace Log
{
    void info(std::string message) {
        std::cout << "[INFO] " << message << std::endl;
    }
    void error(std::string message) {
        std::cout << "[ERROR] " << message << std::endl;
    }
    void debug(std::string message) { 
        std::cout << "[DEBUG] " << message << std::endl;
    }
}