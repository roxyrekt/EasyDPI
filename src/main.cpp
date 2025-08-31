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
    gdpi::ServiceManager::cleanupServices();

    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!scManager) {
        std::cerr << "Servis yöneticisi açılamadı. Hata kodu: " << GetLastError() << "\n";
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
        std::cerr << "Servis oluşturulamadı. Hata kodu: " << GetLastError() << "\n";
        CloseServiceHandle(scManager);
        return;
    }

    SERVICE_DESCRIPTIONA desc;
    desc.lpDescription = (LPSTR)"Passive Deep Packet Inspection blocker and Active DPI circumvention utility";
    ChangeServiceConfig2A(newService, SERVICE_CONFIG_DESCRIPTION, &desc);

    if (!StartServiceA(newService, 0, NULL)) {
        std::cerr << "Servis başlatılamadı. Hata kodu: " << GetLastError() << "\n";
    }
    else {
        std::cout << "Servis başarıyla başlatıldı.\n";
    }

    CloseServiceHandle(newService);
    CloseServiceHandle(scManager);
}

void uninstallGoodByeDPI() {
    system("taskkill /F /IM goodbyedpi.exe > nul 2>&1");

    if (!gdpi::ServiceManager::cleanupServices()) {
        printMessage("Uyarı: Bazı servisler silinemeyebilir.", 12);
    }
    if (gdpi::PathManager::isInstalled()) {
        if (gdpi::FileSystem::removeAll(gdpi::PathManager::getBaseInstallPath())) {
            printMessage("GoodByeDPI başarıyla silindi.", 10);
        }
        else {
            printMessage("GoodByeDPI silinemedi.", 12);
        }
    }
    else {
        printMessage("GoodByeDPI kurulu değil.", 14);
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
            printMessage("İndirme başarısız!", 12);
            return;
        }

        printMessage("İndirme başarıyla tamamlandı!", 10);
        printMessage("Dosyalar çıkartılıyor...", 10);
        if (gdpi::Extractor::extractInnerFolder(zipPath, baseInstallPath)) {
            printMessage("Dosyalar başarıyla çıkartıldı!", 10);
            gdpi::FileSystem::remove(zipPath);
            gdpi::FileSystem::removeAll(tempPath);
        }
        else {
            printMessage("Dosyaları çıkartma başarısız!", 12);
            return;
        }

        if (gdpi::PathManager::isInstalled()) {
            printMessage("İndirme başarıyla doğrulandı!", 10);
        }
        else {
            printMessage("İndirme doğrulanamadı!", 12);
        }
    }
    catch (const std::exception& e) {
        printMessage(std::string("İndirirken hata: ") + e.what(), 12);
    }
}

void testArguments() {
    gdpi::ServiceManager::cleanupServices();
    std::string exePath = gdpi::PathManager::getArchInstallPath() + "\\goodbyedpi.exe";
    std::string bestArgs = gdpi::Tester::findBestArguments(exePath);
    if (!bestArgs.empty()) {
        manageGoodByeDPIService(bestArgs);
        auto path = gdpi::PathManager::getArchInstallPath() + "\\eniyiyontem.txt";
        printMessage("TXT dosyasına kaydediliyor... Konum: " + path, 10);
        std::ofstream configFile(path);
        configFile << bestArgs;
    }
    else {
        printMessage("Uygun yöntem bulunamadı.", 12);
    }
}

int main() {
    setlocale(LC_ALL, "Turkish");
    if (!ensureIsAdmin())
    {
        printMessage("Yonetici olarak baslatmalisiniz!", 12);
        return 1;
    }

    while (true) {
        bool isInstalled = gdpi::PathManager::isInstalled();
        printMessage("\n--- GoodByeDPI ---", 11);
        if (isInstalled) {
            printMessage("GoodbyeDPI kurulu.", 10);
        }
        else {
            printMessage("GoodByeDPI kurulu degil.", 12);
        }
        printMessage("1. GoodByeDPI indir", 14);
        printMessage("2. GoodByeDPI sil", 14);
        printMessage("3. En iyi yontemi ara", 14);
        printMessage("4. Cikis", 14);
        printMessage("Bir secenek secin: ", 7);

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
            printMessage("Cikiliyor...", 10);
            return 0;
        default:
            printMessage("Gecersiz secim, lutfen tekrar deneyin.", 12);
        }
    }
}
