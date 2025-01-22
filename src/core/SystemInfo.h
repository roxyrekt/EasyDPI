#pragma once
#include <string>
#include <windows.h>

namespace gdpi {
    class SystemInfo {
    public:
        static bool isX64();
        static std::string getArchitectureString();
        static std::string getArchitecturePath();
    };
}