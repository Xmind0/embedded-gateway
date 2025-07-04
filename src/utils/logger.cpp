#include "logger.h"
#include <iostream>

namespace Logger {
    void info(const std::string& msg) {
        std::cout << "[INFO] " << msg << std::endl;
    }
    void debug(const std::string& msg) {
        std::cout << "[DEBUG] " << msg << std::endl;
    }
    void error(const std::string& msg) {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
} 