#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "core/DownloadManager.h"
#include "core/Extractor.h"
#include "core/PathManager.h"
#include "core/Tester.h"
#include "core/FileSystem.h"
#include "core/ServiceManager.h"

bool ensureIsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &adminGroup)) {
        if (!CheckTokenMembership(NULL, adminGroup, &isAdmin)) {
            isAdmin = FALSE;
        }
        FreeSid(adminGroup);
    }
    
    if (isAdmin == FALSE)
    {
        char szPath[MAX_PATH];
        if (GetModuleFileNameA(NULL, szPath, MAX_PATH)) {
            SHELLEXECUTEINFOA sei = { sizeof(sei) };
            sei.lpVerb = "runas";
            sei.lpFile = szPath;
            sei.hwnd = NULL;
            sei.nShow = SW_NORMAL;

            if (ShellExecuteExA(&sei)) {
                exit(0);
                return true;
            }
        }
    }
    else if (isAdmin == TRUE)
    {
        return true;
    }
    return false;
}

void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printMessage(const std::string& message, int color) {
    setConsoleColor(color);
    std::cout << message << std::endl;
    setConsoleColor(7);
}

void manageGoodByeDPIService(const std::string& arguments) {
    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!scManager) {
        std::cerr << "Servis y�neticisi a��lamad�. Hata kodu: " << GetLastError() << "\n";
        return;
    }

    SC_HANDLE service = OpenServiceA(scManager, "GoodbyeDPI", SERVICE_ALL_ACCESS);
    if (service) {
        SERVICE_STATUS status;
        ControlService(service, SERVICE_CONTROL_STOP, &status);
        DeleteService(service);
        CloseServiceHandle(service);
    }

    std::string servicePath = gdpi::PathManager::getArchInstallPath() + "\\goodbyedpi.exe";

    std::string fullCommand = "\"" + servicePath + "\" " + arguments;

    SC_HANDLE newService = CreateServiceA(
        scManager,
        "GoodbyeDPI",
        "GoodbyeDPI",
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        fullCommand.c_str(),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    );

    if (!newService) {
        std::cerr << "Servis olu�turulamad�. Hata kodu: " << GetLastError() << "\n";
        CloseServiceHandle(scManager);
        return;
    }

    SERVICE_DESCRIPTIONA desc;
    desc.lpDescription = (LPSTR)"Passive Deep Packet Inspection blocker and Active DPI circumvention utility";
    ChangeServiceConfig2A(newService, SERVICE_CONFIG_DESCRIPTION, &desc);

    if (!StartServiceA(newService, 0, NULL)) {
        std::cerr << "Servis ba�lat�lamad�. Hata kodu: " << GetLastError() << "\n";
    }
    else {
        std::cout << "Servis ba�ar�yla ba�lat�ld�.\n";
    }

    CloseServiceHandle(newService);
    CloseServiceHandle(scManager);
}

void uninstallGoodByeDPI() {
    system("taskkill /F /IM goodbyedpi.exe > nul 2>&1");

    if (!gdpi::ServiceManager::cleanupServices()) {
        printMessage("Uyar�: Baz� servisler silinemeyebilir.", 12);
    }
    if (gdpi::PathManager::isInstalled()) {
        if (gdpi::FileSystem::removeAll(gdpi::PathManager::getBaseInstallPath())) {
            printMessage("GoodByeDPI ba�ar�yla silindi.", 10);
        }
        else {
            printMessage("GoodByeDPI silinemedi.", 12);
        }
    }
    else {
        printMessage("GoodByeDPI kurulu de�il.", 14);
    }
}

void installGoodByeDPI() {
    try {
        if (gdpi::PathManager::isInstalled()) {
            printMessage("GoodByeDPI zaten y�kl�. Tekrar y�klemek mi istiyorsunuz? (y/n): ", 14);
            std::string response;
            std::cin >> response;
            std::cin.ignore();
            if (response != "y" && response != "Y") return;

            uninstallGoodByeDPI();
        }

        std::string baseInstallPath = gdpi::PathManager::getBaseInstallPath();
        std::string tempPath = gdpi::PathManager::getTempPath();
        std::string zipPath = tempPath + "\\goodbyedpi.zip";

        gdpi::PathManager::createDirectories(baseInstallPath);
        gdpi::PathManager::createDirectories(tempPath);

        gdpi::DownloadManager downloader;
        printMessage("En son GoodByeDPI s�r�m� indiriliyor...", 10);
        if (!downloader.downloadLatestRelease(zipPath)) {
            printMessage("�ndirme ba�ar�s�z!", 12);
            return;
        }

        printMessage("�ndirme ba�ar�yla tamamland�!", 10);
        printMessage("Dosyalar ��kart�l�yor...", 10);
        if (gdpi::Extractor::extractInnerFolder(zipPath, baseInstallPath)) {
            printMessage("Dosyalar ba�ar�yla ��kart�ld�!", 10);
            gdpi::FileSystem::remove(zipPath);
            gdpi::FileSystem::removeAll(tempPath);
        }
        else {
            printMessage("Dosyalar� ��kartma ba�ar�s�z!", 12);
            return;
        }

        if (gdpi::PathManager::isInstalled()) {
            printMessage("�ndirme ba�ar�yla do�ruland�!", 10);
        }
        else {
            printMessage("�ndirme do�rulanamad�!", 12);
        }
    }
    catch (const std::exception& e) {
        printMessage(std::string("�ndirirken hata: ") + e.what(), 12);
    }
}

void testArguments() {
    std::string exePath = gdpi::PathManager::getArchInstallPath() + "\\goodbyedpi.exe";
    std::string bestArgs = gdpi::Tester::findBestArguments(exePath);
    if (!bestArgs.empty()) {
        // printMessage("En iyi y�ntem bulundu: " + bestArgs, 10);
        manageGoodByeDPIService(bestArgs);
        auto path = gdpi::PathManager::getArchInstallPath() + "\\eniyiyontem.txt";
        printMessage("TXT dosyas�na kaydediliyor... Konum: " + path, 10);
        std::ofstream configFile(path);
        configFile << bestArgs;
    }
    else {
        printMessage("Uygun y�ntem bulunamad�.", 12);
    }
}

int main() {
    setlocale(LC_ALL, "Turkish");
    if (!ensureIsAdmin())
    {
        printMessage("Y�netici olarak ba�latmal�s�n�z!", 12);
        return 1;
    }

    while (true) {
        bool isInstalled = gdpi::PathManager::isInstalled();
        printMessage("DNS de�i�tirmeyi unutma, yoksa bir i�e yaramaz. :)", 14);
        printMessage("\n--- GoodByeDPI ---", 11);
        if (isInstalled) {
            printMessage("GoodbyeDPI kurulu.", 10);
        }
        else {
            printMessage("GoodByeDPI kurulu de�il.", 12);
        }
        printMessage("1. GoodByeDPI indir", 14);
        printMessage("2. GoodByeDPI sil", 14);
        printMessage("3. En iyi y�ntemi ara", 14);
        printMessage("4. ��k��", 14);
        printMessage("Bir se�enek se�in: ", 7);

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        system("cls");

        switch (choice) {
        case 1:
            installGoodByeDPI();
            break;
        case 2:
            uninstallGoodByeDPI();
            break;
        case 3:
            if (isInstalled)
                testArguments();
            break;
        case 4:
            printMessage("��k�l�yor...", 10);
            return 0;
        default:
            printMessage("Ge�ersiz se�im, l�tfen tekrar deneyin.", 12);
        }
    }
}
