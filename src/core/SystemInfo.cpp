#include "SystemInfo.h"

namespace gdpi {
    bool SystemInfo::isX64() {
#ifdef _WIN64
        return true;
#else
        BOOL bIsWow64 = FALSE;
        typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

        HMODULE hKernel32 = GetModuleHandle(TEXT("kernel32"));
        if (!hKernel32) {
            return false; 
        }

        LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)
            GetProcAddress(hKernel32, "IsWow64Process");

        if (fnIsWow64Process) {
            if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
                return false;
            }
        }
        return bIsWow64 != FALSE;
#endif
    }

    std::string SystemInfo::getArchitectureString() {
        return isX64() ? "x86_64" : "x86";
    }

    std::string SystemInfo::getArchitecturePath() {
        return isX64() ? "x86_64" : "x86";
    }
}