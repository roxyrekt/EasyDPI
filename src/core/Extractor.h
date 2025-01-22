#pragma once
#include <string>
#include <windows.h>
#include <shlobj.h>
#include <shldisp.h>
#include <comdef.h>
#include "FileSystem.h"

namespace gdpi {
    class Extractor {
    public:
        static bool extractInnerFolder(const std::string& zipPath, const std::string& outputPath);
    private:
        static bool extract(const std::string& zipPath, const std::string& outputPath);
        static std::wstring stringToWString(const std::string& str);
        static std::string findFirstSubDirectory(const std::string& path);
        static bool moveDirectoryContents(const std::string& sourceDir, const std::string& destDir);
    };
}
