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
        std::cerr << "Servis yöneticisi açýlamadý. Hata kodu: " << GetLastError() << "\n";
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
        std::cerr << "Servis oluþturulamadý. Hata kodu: " << GetLastError() << "\n";
        CloseServiceHandle(scManager);
        return;
    }

    SERVICE_DESCRIPTIONA desc;
    desc.lpDescription = (LPSTR)"Passive Deep Packet Inspection blocker and Active DPI circumvention utility";
    ChangeServiceConfig2A(newService, SERVICE_CONFIG_DESCRIPTION, &desc);

    if (!StartServiceA(newService, 0, NULL)) {
        std::cerr << "Servis baþlatýlamadý. Hata kodu: " << GetLastError() << "\n";
    }
    else {
        std::cout << "Servis baþarýyla baþlatýldý.\n";
    }

    CloseServiceHandle(newService);
    CloseServiceHandle(scManager);
}

void uninstallGoodByeDPI() {
    system("taskkill /F /IM goodbyedpi.exe > nul 2>&1");

    if (!gdpi::ServiceManager::cleanupServices()) {
        printMessage("Uyarý: Bazý servisler silinemeyebilir.", 12);
    }
    if (gdpi::PathManager::isInstalled()) {
        if (gdpi::FileSystem::removeAll(gdpi::PathManager::getBaseInstallPath())) {
            printMessage("GoodByeDPI baþarýyla silindi.", 10);
        }
        else {
            printMessage("GoodByeDPI silinemedi.", 12);
        }
    }
    else {
        printMessage("GoodByeDPI kurulu deðil.", 14);
    }
}

void installGoodByeDPI() {
    try {
        if (gdpi::PathManager::isInstalled()) {
            printMessage("GoodByeDPI zaten yüklü. Tekrar yüklemek mi istiyorsunuz? (y/n): ", 14);
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
        printMessage("En son GoodByeDPI sürümü indiriliyor...", 10);
        if (!downloader.downloadLatestRelease(zipPath)) {
            printMessage("Ýndirme baþarýsýz!", 12);
            return;
        }

        printMessage("Ýndirme baþarýyla tamamlandý!", 10);
        printMessage("Dosyalar çýkartýlýyor...", 10);
        if (gdpi::Extractor::extractInnerFolder(zipPath, baseInstallPath)) {
            printMessage("Dosyalar baþarýyla çýkartýldý!", 10);
            gdpi::FileSystem::remove(zipPath);
            gdpi::FileSystem::removeAll(tempPath);
        }
        else {
            printMessage("Dosyalarý çýkartma baþarýsýz!", 12);
            return;
        }

        if (gdpi::PathManager::isInstalled()) {
            printMessage("Ýndirme baþarýyla doðrulandý!", 10);
        }
        else {
            printMessage("Ýndirme doðrulanamadý!", 12);
        }
    }
    catch (const std::exception& e) {
        printMessage(std::string("Ýndirirken hata: ") + e.what(), 12);
    }
}

void testArguments() {
    std::string exePath = gdpi::PathManager::getArchInstallPath() + "\\goodbyedpi.exe";
    std::string bestArgs = gdpi::Tester::findBestArguments(exePath);
    if (!bestArgs.empty()) {
        // printMessage("En iyi yöntem bulundu: " + bestArgs, 10);
        manageGoodByeDPIService(bestArgs);
        auto path = gdpi::PathManager::getArchInstallPath() + "\\eniyiyontem.txt";
        printMessage("TXT dosyasýna kaydediliyor... Konum: " + path, 10);
        std::ofstream configFile(path);
        configFile << bestArgs;
    }
    else {
        printMessage("Uygun yöntem bulunamadý.", 12);
    }
}

int main() {
    setlocale(LC_ALL, "Turkish");
    if (!ensureIsAdmin())
    {
        printMessage("Yönetici olarak baþlatmalýsýnýz!", 12);
        return 1;
    }

    while (true) {
        bool isInstalled = gdpi::PathManager::isInstalled();
        printMessage("DNS deðiþtirmeyi unutma, yoksa bir iþe yaramaz. :)", 14);
        printMessage("\n--- GoodByeDPI ---", 11);
        if (isInstalled) {
            printMessage("GoodbyeDPI kurulu.", 10);
        }
        else {
            printMessage("GoodByeDPI kurulu deðil.", 12);
        }
        printMessage("1. GoodByeDPI indir", 14);
        printMessage("2. GoodByeDPI sil", 14);
        printMessage("3. En iyi yöntemi ara", 14);
        printMessage("4. Çýkýþ", 14);
        printMessage("Bir seçenek seçin: ", 7);

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
            printMessage("Çýkýlýyor...", 10);
            return 0;
        default:
            printMessage("Geçersiz seçim, lütfen tekrar deneyin.", 12);
        }
    }
}
