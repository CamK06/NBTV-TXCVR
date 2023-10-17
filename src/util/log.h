#pragma once

#include <iostream>

namespace Log
{
    void info(std::string message);
    void error(std::string message);
    void debug(std::string message);
}