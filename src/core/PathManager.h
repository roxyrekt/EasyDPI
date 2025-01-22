#pragma once
#include <string>
#include <windows.h>
#include <shlobj.h>
#include "SystemInfo.h"

namespace gdpi {
    class PathManager {
    public:
        static std::string getBaseInstallPath();
        static std::string getArchInstallPath();
        static std::string getTempPath();
        static bool createDirectories(const std::string& path);
        static bool isInstalled();
    };
}