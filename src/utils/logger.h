#pragma once

#include <string>

namespace Logger {
    void info(const std::string& msg);
    void debug(const std::string& msg);
    void error(const std::string& msg);
} 