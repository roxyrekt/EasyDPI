#include "PathManager.h"
#include "FileSystem.h"

namespace gdpi {
    std::string PathManager::getBaseInstallPath() {
        wchar_t* localAppData = nullptr;
        std::string path;

        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &localAppData))) {
            std::wstring widePath(localAppData);
            path = std::string(widePath.begin(), widePath.end());
            CoTaskMemFree(localAppData);
        }

        return path + "\\GoodByeDPI";
    }

    std::string PathManager::getArchInstallPath() {
        return getBaseInstallPath() + "\\" + SystemInfo::getArchitecturePath();
    }

    std::string PathManager::getTempPath() {
        return getBaseInstallPath() + "\\temp";
    }

    bool PathManager::createDirectories(const std::string& path) {
        return FileSystem::createDirectories(path);
    }

    bool PathManager::isInstalled() {
        std::string installPath = getArchInstallPath();
        return FileSystem::exists(installPath + "\\goodbyedpi.exe") &&
            FileSystem::exists(installPath + "\\WinDivert.dll");
    }
}