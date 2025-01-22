#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <thread>
#include <chrono>

namespace gdpi {
    class Tester {
    public:
        static std::string findBestArguments(const std::string& exePath);

    private:
        static const int C_RED = 12;
        static const int C_GREEN = 10;
        static const int C_YELLOW = 14;
        static const int C_WHITE = 15;

        static constexpr double STARTUP_DELAY = 0.04;
        static constexpr double SHUTDOWN_DELAY = 0.04;
        static constexpr double TEST_DELAY = 0.02;

        static HANDLE processHandle;
        static const std::vector<std::string> TEST_ARGUMENTS;
        static const std::vector<std::string> TEST_URLS;

        static void setTextColor(int color);
        static void delay(double seconds, bool print = false);
        static bool testConnection(const std::string& url);
        static bool startGoodbyeDPI(const std::string& exePath, const std::string& args);
        static void stopGoodbyeDPI();
        static bool testArgument(const std::string& exePath, const std::string& args);

        struct TestResult {
            std::string url;
            bool success;
            TestResult(const std::string& u, bool s) : url(u), success(s) {}
        };

        static void printResults(const std::vector<TestResult>& results, const std::string& args = "");
        static int calculateSuccessRate(const std::vector<TestResult>& results);
    };
}