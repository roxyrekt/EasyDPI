#pragma once
#include <string>
#include <windows.h>

namespace gdpi {
    class ServiceManager {
    public:
        static bool stopAndDeleteService(const std::string& serviceName);
        static bool cleanupServices();
    };
}