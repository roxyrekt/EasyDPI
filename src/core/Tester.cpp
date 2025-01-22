#include "Tester.h"

namespace gdpi {

	const std::vector<std::string> Tester::TEST_ARGUMENTS = {
	"--native-frag",
	"--reverse-frag",
	"--native-frag --wrong-seq",
	"--reverse-frag --wrong-seq",
	"--native-frag --wrong-chksum",
	"--reverse-frag --wrong-chksum",
	"--native-frag --wrong-seq --wrong-chksum",
	"--reverse-frag --wrong-seq --wrong-chksum",
	"--frag-by-sni",
	"--native-frag --frag-by-sni",
	"--reverse-frag --frag-by-sni",
	"--native-frag --wrong-seq --frag-by-sni",
	"--reverse-frag --wrong-seq --frag-by-sni",
	"--native-frag --wrong-chksum --frag-by-sni",
	"--reverse-frag --wrong-chksum --frag-by-sni",
	"--native-frag --wrong-seq --wrong-chksum --frag-by-sni",
	"--reverse-frag --wrong-seq --wrong-chksum --frag-by-sni",
	"--set-ttl 3",
	"--native-frag --set-ttl 3",
	"--reverse-frag --set-ttl 3",
	"--native-frag --wrong-seq --set-ttl 3",
	"--reverse-frag --wrong-seq --set-ttl 3",
	"--native-frag --wrong-chksum --set-ttl 3",
	"--reverse-frag --wrong-chksum --set-ttl 3",
	"--native-frag --wrong-seq --wrong-chksum --set-ttl 3",
	"--reverse-frag --wrong-seq --wrong-chksum --set-ttl 3",
	"--frag-by-sni --set-ttl 3",
	"--native-frag --frag-by-sni --set-ttl 3",
	"--reverse-frag --frag-by-sni --set-ttl 3",
	"--native-frag --wrong-seq --frag-by-sni --set-ttl 3",
	"--reverse-frag --wrong-seq --frag-by-sni --set-ttl 3",
	"--native-frag --wrong-chksum --frag-by-sni --set-ttl 3",
	"--reverse-frag --wrong-chksum --frag-by-sni --set-ttl 3",
	"--native-frag --wrong-seq --wrong-chksum --frag-by-sni --set-ttl 3",
	"--reverse-frag --wrong-seq --wrong-chksum --frag-by-sni --set-ttl 3",
	"--set-ttl 4",
	"--native-frag --set-ttl 4",
	"--reverse-frag --set-ttl 4",
	"--native-frag --wrong-seq --set-ttl 4",
	"--reverse-frag --wrong-seq --set-ttl 4",
	"--native-frag --wrong-chksum --set-ttl 4",
	"--reverse-frag --wrong-chksum --set-ttl 4",
	"--native-frag --wrong-seq --wrong-chksum --set-ttl 4",
	"--reverse-frag --wrong-seq --wrong-chksum --set-ttl 4",
	"--frag-by-sni --set-ttl 4",
	"--native-frag --frag-by-sni --set-ttl 4",
	"--reverse-frag --frag-by-sni --set-ttl 4",
	"--native-frag --wrong-seq --frag-by-sni --set-ttl 4",
	"--reverse-frag --wrong-seq --frag-by-sni --set-ttl 4",
	"--native-frag --wrong-chksum --frag-by-sni --set-ttl 4",
	"--reverse-frag --wrong-chksum --frag-by-sni --set-ttl 4",
	"--native-frag --wrong-seq --wrong-chksum --frag-by-sni --set-ttl 4",
	"--reverse-frag --wrong-seq --wrong-chksum --frag-by-sni --set-ttl 4",
	"--set-ttl 5",
	"--native-frag --set-ttl 5",
	"--reverse-frag --set-ttl 5",
	"--native-frag --wrong-seq --set-ttl 5",
	"--reverse-frag --wrong-seq --set-ttl 5",
	"--native-frag --wrong-chksum --set-ttl 5",
	"--reverse-frag --wrong-chksum --set-ttl 5",
	"--native-frag --wrong-seq --wrong-chksum --set-ttl 5",
	"--reverse-frag --wrong-seq --wrong-chksum --set-ttl 5",
	"--frag-by-sni --set-ttl 5",
	"--native-frag --frag-by-sni --set-ttl 5",
	"--reverse-frag --frag-by-sni --set-ttl 5",
	"--native-frag --wrong-seq --frag-by-sni --set-ttl 5",
	"--reverse-frag --wrong-seq --frag-by-sni --set-ttl 5",
	"--native-frag --wrong-chksum --frag-by-sni --set-ttl 5",
	"--reverse-frag --wrong-chksum --frag-by-sni --set-ttl 5",
	"--native-frag --wrong-seq --wrong-chksum --frag-by-sni --set-ttl 5",
	"--reverse-frag --wrong-seq --wrong-chksum --frag-by-sni --set-ttl 5"
	};

	const std::vector<std::string> Tester::TEST_URLS = {
	"https://www.pastebin.com",
	"https://www.roblox.com",
	"https://www.discord.com",
	"https://www.discord.gg",
	"https://www.discordapp.com",
	"https://www.reddit.com",
	"https://www.youtube.com",
	"https://www.instagram.com",
	};

	HANDLE Tester::processHandle = NULL;

	void Tester::setTextColor(int color) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	}

	void Tester::delay(double seconds, bool print) {
		if (print) {
			std::cout << seconds << " saniye bekleniyor...\n";
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
	}

	bool Tester::testConnection(const std::string& url) {
		HINTERNET hSession = WinHttpOpen(L"GBDPI-Tester/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0);

		if (!hSession) return false;

		bool result = false;
		std::wstring wideUrl(url.begin(), url.end());
		URL_COMPONENTS urlComp = { sizeof(urlComp) };
		urlComp.dwHostNameLength = (DWORD)-1;
		urlComp.dwUrlPathLength = (DWORD)-1;

		if (WinHttpCrackUrl(wideUrl.c_str(), 0, 0, &urlComp) &&
			urlComp.lpszHostName && urlComp.lpszUrlPath) {

			std::wstring host(urlComp.lpszHostName, urlComp.dwHostNameLength);
			std::wstring path(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);

			if (!host.empty()) {
				HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(),
					urlComp.nPort, 0);

				if (hConnect) {
					HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET",
						path.empty() ? L"/" : path.c_str(),
						NULL, WINHTTP_NO_REFERER,
						WINHTTP_DEFAULT_ACCEPT_TYPES,
						WINHTTP_FLAG_SECURE);

					if (hRequest) {
						result = WinHttpSendRequest(hRequest,
							WINHTTP_NO_ADDITIONAL_HEADERS, 0,
							WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
							WinHttpReceiveResponse(hRequest, NULL);
						WinHttpCloseHandle(hRequest);
					}
					WinHttpCloseHandle(hConnect);
				}
			}
		}

		WinHttpCloseHandle(hSession);
		return result;
	}

	bool Tester::startGoodbyeDPI(const std::string& exePath, const std::string& args) {
		char commandLine[MAX_PATH * 2];
		if (snprintf(commandLine, sizeof(commandLine), "\"%s\" %s",
			exePath.c_str(), args.c_str()) >= sizeof(commandLine)) {
			std::cerr << "Komut satýrý çok uzun!\n";
			return false;
		}

		STARTUPINFOA si = { sizeof(si) };
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		PROCESS_INFORMATION pi;

		if (CreateProcessA(NULL, commandLine, NULL, NULL, FALSE,
			CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
			CloseHandle(pi.hThread);
			processHandle = pi.hProcess;
			return true;
		}

		DWORD error = GetLastError();
		std::cerr << "CreateProcess baþarýsýz hata " << error << std::endl;
		return false;
	}

	void Tester::stopGoodbyeDPI() {
		if (processHandle) {
			TerminateProcess(processHandle, 0);
			CloseHandle(processHandle);
			processHandle = NULL;
			delay(SHUTDOWN_DELAY);
		}
	}

	void Tester::printResults(const std::vector<TestResult>& results, const std::string& args) {
		if (!args.empty()) {
			// std::cout << "\nSonuçlar yöntem için: " << args << std::endl;
		}
		std::cout << "----------------------------------------\n";

		for (const auto& result : results) {
			setTextColor(result.success ? C_GREEN : C_RED);
			std::cout << (result.success ? "[BAÞARILI] " : "[BAÞARSIZ] ") << result.url << std::endl;
		}

		setTextColor(C_WHITE);
		std::cout << "----------------------------------------\n";

		int successRate = calculateSuccessRate(results);
		std::cout << "Baþarý oraný: %" << successRate << "\n";
		std::cout << "----------------------------------------\n";
	}

	int Tester::calculateSuccessRate(const std::vector<TestResult>& results) {
		int successCount = 0;
		for (const auto& result : results) {
			if (result.success) successCount++;
		}
		return (successCount * 100) / results.size();
	}

	bool Tester::testArgument(const std::string& exePath, const std::string& args) {
		if (!startGoodbyeDPI(exePath, args)) {
			return false;
		}

		delay(STARTUP_DELAY);
		std::vector<TestResult> results;

		for (const auto& url : TEST_URLS) {
			bool success = testConnection(url);
			results.emplace_back(url, success);
			delay(TEST_DELAY, false);
		}

		printResults(results, args);
		stopGoodbyeDPI();

		return calculateSuccessRate(results) > 50;
	}

	std::string Tester::findBestArguments(const std::string& exePath) {
		std::cout << "GoodByeDPI yöntemleri deneniyor...\n";
		std::cout << "\nGoodByeDPI olmadan deneniyor...\n";
		delay(TEST_DELAY);

		std::vector<TestResult> initialResults;
		for (const auto& url : TEST_URLS) {
			initialResults.emplace_back(url, testConnection(url));
			delay(TEST_DELAY);
		}

		int rate = calculateSuccessRate(initialResults);
		printResults(initialResults, "Test Sonuçlarý");

		if (rate == 100) {
			setTextColor(C_GREEN);
			std::cout << "\nZaten baþarý oraný %100.\n";
			setTextColor(C_WHITE);
			return std::string("");
		}

		std::cout << "\nGoodByeDPI ile deneniyor...\n";
		delay(TEST_DELAY);

		std::string bestArgs;
		int bestSuccessRate = 0;

		for (const auto& args1 : TEST_ARGUMENTS) {
			auto args = "-e 1 -q " + args1;
			std::cout << "\nDenenen yöntem: " << args << std::endl;

			if (!startGoodbyeDPI(exePath, args)) {
				continue;
			}

			delay(STARTUP_DELAY);
			std::vector<TestResult> results;

			for (const auto& url : TEST_URLS) {
				bool success = testConnection(url);
				results.emplace_back(url, success);
				delay(TEST_DELAY, false);
			}

			printResults(results, args);
			stopGoodbyeDPI();

			int currentSuccessRate = calculateSuccessRate(results);

			if (currentSuccessRate == 100) {
				setTextColor(C_GREEN);
				std::cout << "\nEn iyi yöntem bulundu (%100 baþarý oraný): " << args << std::endl;
				setTextColor(C_WHITE);
				return args;
			}

			if (currentSuccessRate > bestSuccessRate) {
				bestSuccessRate = currentSuccessRate;
				bestArgs = args;
			}

			delay(TEST_DELAY);
		}

		if (!bestArgs.empty()) {
			setTextColor(C_YELLOW);
			std::cout << "\nEn iyi yöntem (%" << bestSuccessRate << " baþarý oraný): " << bestArgs << std::endl;
			setTextColor(C_WHITE);
			return bestArgs;
		}

		setTextColor(C_RED);
		std::cout << "\nÇalýþan yöntem bulunamadý!\n";
		setTextColor(C_WHITE);
		return std::string("");
	}
}