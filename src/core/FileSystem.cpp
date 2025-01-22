#include "FileSystem.h"
#include <shlobj.h>

namespace gdpi {
    bool FileSystem::createDirectories(const std::string& path) {
        std::string currentPath;
        size_t pos = 0;

        while ((pos = path.find('\\', pos)) != std::string::npos) {
            currentPath = path.substr(0, pos);
            CreateDirectoryA(currentPath.c_str(), NULL);
            pos++;
        }

        return CreateDirectoryA(path.c_str(), NULL) ||
            GetLastError() == ERROR_ALREADY_EXISTS;
    }

    bool FileSystem::removeAll(const std::string& path) {
        std::string searchPath = path + "\\*";
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

        if (hFind == INVALID_HANDLE_VALUE) {
            return false;
        }

        do {
            if (strcmp(findData.cFileName, ".") != 0 &&
                strcmp(findData.cFileName, "..") != 0) {
                std::string filePath = path + "\\" + findData.cFileName;

                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    removeAll(filePath);
                    RemoveDirectoryA(filePath.c_str());
                }
                else {
                    DeleteFileA(filePath.c_str());
                }
            }
        } while (FindNextFileA(hFind, &findData));

        FindClose(hFind);
        return RemoveDirectoryA(path.c_str());
    }

    bool FileSystem::exists(const std::string& path) {
        DWORD attr = GetFileAttributesA(path.c_str());
        return (attr != INVALID_FILE_ATTRIBUTES);
    }

    bool FileSystem::remove(const std::string& path) {
        DWORD attr = GetFileAttributesA(path.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES) {
            return false;
        }

        if (attr & FILE_ATTRIBUTE_DIRECTORY) {
            return RemoveDirectoryA(path.c_str());
        }
        else {
            return DeleteFileA(path.c_str());
        }
    }
}