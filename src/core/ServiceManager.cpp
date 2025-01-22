#include "ServiceManager.h"
#include <iostream>

namespace gdpi {
    bool ServiceManager::stopAndDeleteService(const std::string& serviceName) {
        SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (!scManager) {
            return false;
        }

        SC_HANDLE service = OpenServiceA(scManager, serviceName.c_str(), SERVICE_ALL_ACCESS);
        if (service) {
            SERVICE_STATUS_PROCESS ssp;
            DWORD bytesNeeded;
            if (QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
                (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {

                if (ssp.dwCurrentState != SERVICE_STOPPED) {
                    std::cout << "Servis durduruluyor: " << serviceName << std::endl;
                    SERVICE_STATUS status;
                    ControlService(service, SERVICE_CONTROL_STOP, &status);

                    int attempts = 0;
                    while (ssp.dwCurrentState != SERVICE_STOPPED && attempts < 10) {
                        Sleep(1000);
                        bool a = QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
                            (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded);
                        attempts++;
                    }
                }
            }

            std::cout << "Servis siliniyor: " << serviceName << std::endl;
            DeleteService(service);
            CloseServiceHandle(service);
        }

        CloseServiceHandle(scManager);
        return true;
    }

    bool ServiceManager::cleanupServices() {
        std::cout << "GoodByeDPI servisleri siliniyor...\n";
        bool success = true;

        success &= stopAndDeleteService("GoodbyeDPI");
        success &= stopAndDeleteService("WinDivert");
        success &= stopAndDeleteService("WinDivert14");

        return success;
    }
}