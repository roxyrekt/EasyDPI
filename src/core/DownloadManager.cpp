#include "DownloadManager.h"
#include <stdexcept>
#include <fstream>
#include <iostream>

namespace gdpi {
	std::string DownloadManager::httpRequest(const wchar_t* host, const wchar_t* path) {
		std::string response;
		HINTERNET hSession = WinHttpOpen(L"GBDPI-M/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0);

		if (hSession) {
			HINTERNET hConnect = WinHttpConnect(hSession, host,
				INTERNET_DEFAULT_HTTPS_PORT, 0);

			if (hConnect) {
				HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path,
					NULL, WINHTTP_NO_REFERER,
					WINHTTP_DEFAULT_ACCEPT_TYPES,
					WINHTTP_FLAG_SECURE);

				if (hRequest) {
					LPCWSTR headers = L"User-Agent: GDBPI-M/1.0\r\n";
					DWORD header_len = lstrlenW(headers);
					WinHttpAddRequestHeaders(hRequest, headers, header_len, WINHTTP_ADDREQ_FLAG_ADD);

					if (WinHttpSendRequest(hRequest,
						WINHTTP_NO_ADDITIONAL_HEADERS, 0,
						WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
						WinHttpReceiveResponse(hRequest, NULL)) {

						DWORD size = 0;
						do {
							size = 0;
							if (!WinHttpQueryDataAvailable(hRequest, &size)) {
								break;
							}

							if (size == 0) {
								break;
							}

							std::vector<char> buffer(size);
							DWORD downloaded = 0;

							if (WinHttpReadData(hRequest, buffer.data(), size, &downloaded)) {
								response.append(buffer.data(), downloaded);
							}
						} while (size > 0);
					}
					WinHttpCloseHandle(hRequest);
				}
				WinHttpCloseHandle(hConnect);
			}
			WinHttpCloseHandle(hSession);
		}

		return response;
	}

	std::string DownloadManager::getLatestReleaseUrl() {
		std::string response = httpRequest(L"api.github.com",
			L"/repos/ValdikSS/GoodbyeDPI/releases");

		try {
			auto json = nlohmann::json::parse(response);
			for (const auto& release : json) {
				for (const auto& asset : release["assets"]) {
					std::string name = asset["name"].get<std::string>();
					if (name.find(".zip") != std::string::npos) {
						return asset["browser_download_url"].get<std::string>();
					}
				}
			}
		}
		catch (const nlohmann::json::exception& e) {
			throw std::runtime_error("JSON çözülürken hata: " + std::string(e.what()));
		}

		throw std::runtime_error("Uygun sürüm bulunamadý");
	}

	bool DownloadManager::downloadFile(const std::wstring& url, const std::string& outputPath) {
		URL_COMPONENTS urlComp = { 0 };
		urlComp.dwStructSize = sizeof(urlComp);

		wchar_t hostName[256] = { 0 };
		wchar_t urlPath[2048] = { 0 };
		urlComp.lpszHostName = hostName;
		urlComp.dwHostNameLength = sizeof(hostName) / sizeof(wchar_t);
		urlComp.lpszUrlPath = urlPath;
		urlComp.dwUrlPathLength = sizeof(urlPath) / sizeof(wchar_t);

		if (!WinHttpCrackUrl(url.c_str(), url.length(), 0, &urlComp)) {
			return false;
		}

		std::ofstream outFile(outputPath, std::ios::binary);
		if (!outFile) {
			return false;
		}

		bool success = false;

		HINTERNET hSession = WinHttpOpen(L"GBDPI-M/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0);

		if (hSession) {
			HINTERNET hConnect = WinHttpConnect(hSession, hostName,
				urlComp.nPort, 0);

			if (hConnect) {
				HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", urlPath,
					NULL, WINHTTP_NO_REFERER,
					WINHTTP_DEFAULT_ACCEPT_TYPES,
					WINHTTP_FLAG_SECURE);

				if (hRequest) {
					if (WinHttpSendRequest(hRequest,
						WINHTTP_NO_ADDITIONAL_HEADERS, 0,
						WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
						WinHttpReceiveResponse(hRequest, NULL)) {

						DWORD size = 0;
						do {
							size = 0;
							if (!WinHttpQueryDataAvailable(hRequest, &size)) {
								break;
							}

							if (size == 0) {
								break;
							}

							std::vector<char> buffer(size);
							DWORD downloaded = 0;

							if (WinHttpReadData(hRequest, buffer.data(), size, &downloaded)) {
								outFile.write(buffer.data(), downloaded);
								success = true;
							}
							else {
								success = false;
								break;
							}
						} while (size > 0);
					}
					WinHttpCloseHandle(hRequest);
				}
				WinHttpCloseHandle(hConnect);
			}
			WinHttpCloseHandle(hSession);
		}

		outFile.close();
		return success;
	}

	bool DownloadManager::downloadLatestRelease(const std::string& outputPath) {
		try {
			std::string downloadUrl = getLatestReleaseUrl();
			std::cout << "Ýndiriliyor: " << downloadUrl << std::endl;

			std::wstring wideUrl(downloadUrl.begin(), downloadUrl.end());
			return downloadFile(wideUrl, outputPath);
		}
		catch (const std::exception& e) {
			std::cerr << "Hata: " << e.what() << std::endl;
			return false;
		}
	}
}