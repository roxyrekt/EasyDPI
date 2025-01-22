#pragma once
#include <string>
#include <windows.h>

namespace gdpi {
    class FileSystem {
    public:
        static bool createDirectories(const std::string& path);
        static bool removeAll(const std::string& path);
        static bool exists(const std::string& path);
        static bool remove(const std::string& path);
    };
}