#pragma once
#include <string>
#include <windows.h>
#include <winhttp.h>
#include <nlohmann/json.hpp>

#pragma comment(lib, "winhttp.lib")

namespace gdpi {
    class DownloadManager {
    public:
        DownloadManager() = default;
        ~DownloadManager() = default;

        bool downloadLatestRelease(const std::string& outputPath);

    private:
        std::string getLatestReleaseUrl();
        bool downloadFile(const std::wstring& url, const std::string& outputPath);
        std::string httpRequest(const wchar_t* host, const wchar_t* path);
    };
}