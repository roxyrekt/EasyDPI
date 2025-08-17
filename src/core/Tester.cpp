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
		"https://www.youtube.com",
		"https://www.instagram.com",
		"https://www.pastebin.com",
		"https://www.roblox.com",
		"https://www.discord.com"
	};

	HANDLE Tester::processHandle = NULL;
	HINTERNET Tester::hSession = NULL;
	std::mutex Tester::resultsMutex;

	void Tester::init() {
		hSession = WinHttpOpen(L"GBDPI-Tester/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0);
		if (hSession) {
			WinHttpSetTimeouts(hSession, 500, 500, 500, 500);
		}
	}

	void Tester::cleanup() {
		if (hSession) {
			WinHttpCloseHandle(hSession);
			hSession = NULL;
		}
	}

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

		return result;
	}

	bool Tester::startGoodbyeDPI(const std::string& exePath, const std::string& args) {
		char commandLine[MAX_PATH * 2];
		if (snprintf(commandLine, sizeof(commandLine), "\"%s\" %s",
			exePath.c_str(), args.c_str()) >= sizeof(commandLine)) {
			std::cerr << "Komut satiri cok uzun!\n";
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
		std::cerr << "CreateProcess basarisiz hata " << error << std::endl;
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
			// std::cout << "\nSonu�lar y�ntem i�in: " << args << std::endl;
		}
		std::cout << "----------------------------------------\n";

		for (const auto& result : results) {
			setTextColor(result.success ? C_GREEN : C_RED);
			std::cout << (result.success ? "[BASARILI] " : "[BASARSIZ] ") << result.url << std::endl;
		}

		setTextColor(C_WHITE);
		std::cout << "----------------------------------------\n";

		int successRate = calculateSuccessRate(results);
		std::cout << "Basari orani: %" << successRate << "\n";
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

		std::vector<std::future<TestResult>> futures;
		for (const auto& url : TEST_URLS) {
			futures.emplace_back(std::async(std::launch::async, [url]() {
				return TestResult(url, testConnection(url));
				}));
		}

		for (auto& future : futures) {
			results.emplace_back(future.get());
		}

		printResults(results, args);
		stopGoodbyeDPI();

		return calculateSuccessRate(results) > 50;
	}

	std::string Tester::findBestArguments(const std::string& exePath) {
		init();
		std::cout << "GoodByeDPI yontemleri deneniyor...\n";
		std::cout << "\nGoodByeDPI olmadan deneniyor...\n";
		delay(TEST_DELAY);

		std::vector<TestResult> initialResults;
		std::vector<std::future<TestResult>> initialFutures;
		for (const auto& url : TEST_URLS) {
			initialFutures.emplace_back(std::async(std::launch::async, [url]() {
				return TestResult(url, testConnection(url));
				}));
		}
		for (auto& future : initialFutures) {
			initialResults.emplace_back(future.get());
		}

		int rate = calculateSuccessRate(initialResults);
		printResults(initialResults, "Test Sonuclari");

		if (rate == 100) {
			setTextColor(C_GREEN);
			std::cout << "\nZaten basarı orani %100.\n";
			setTextColor(C_WHITE);
			return std::string("");
		}

		std::cout << "\nGoodByeDPI ile deneniyor...\n";
		delay(TEST_DELAY);

		std::string bestArgs;
		int bestSuccessRate = 0;

		for (const auto& args1 : TEST_ARGUMENTS) {
			auto args = "-e 1 " + args1 + " --dns-addr 1.1.1.1 --dns-port 53 --dnsv6-addr 2606:4700:4700::1111 --dnsv6-port 53";
			std::cout << "\nDenenen yontem: " << args << std::endl;

			if (!startGoodbyeDPI(exePath, args)) {
				continue;
			}

			delay(STARTUP_DELAY);
			std::vector<TestResult> results;

			std::vector<std::future<TestResult>> futures;
			for (const auto& url : TEST_URLS) {
				futures.emplace_back(std::async(std::launch::async, [url]() {
					return TestResult(url, testConnection(url));
					}));
			}

			for (auto& future : futures) {
				results.emplace_back(future.get());
			}

			printResults(results, args);
			stopGoodbyeDPI();

			int currentSuccessRate = calculateSuccessRate(results);

			if (currentSuccessRate == 100) {
				setTextColor(C_GREEN);
				std::cout << "\nEn iyi yontem bulundu (%100 basari orani): " << args << std::endl;
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
			std::cout << "\nEn iyi yontem (%" << bestSuccessRate << " basari orani): " << bestArgs << std::endl;
			setTextColor(C_WHITE);
			return bestArgs;
		}

		setTextColor(C_RED);
		std::cout << "\nCalisan yontem bulunamadi!\n";
		setTextColor(C_WHITE);
		cleanup();
		return std::string("");
	}
}