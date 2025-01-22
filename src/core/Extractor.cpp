#include "Extractor.h"
#include <iostream>

namespace gdpi {
    std::wstring Extractor::stringToWString(const std::string& str) {
        if (str.empty()) return std::wstring();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    std::string Extractor::findFirstSubDirectory(const std::string& path) {
        std::string searchPath = path + "\\*";
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

        if (hFind == INVALID_HANDLE_VALUE) {
            return "";
        }

        std::string result;
        do {
            if (strcmp(findData.cFileName, ".") != 0 &&
                strcmp(findData.cFileName, "..") != 0) {
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    result = path + "\\" + findData.cFileName;
                    break;
                }
            }
        } while (FindNextFileA(hFind, &findData));

        FindClose(hFind);
        return result;
    }

    bool Extractor::moveDirectoryContents(const std::string& sourceDir, const std::string& destDir) {
        std::string searchPath = sourceDir + "\\*";
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

        if (hFind == INVALID_HANDLE_VALUE) {
            return false;
        }

        bool success = true;
        do {
            if (strcmp(findData.cFileName, ".") != 0 &&
                strcmp(findData.cFileName, "..") != 0) {
                std::string sourcePath = sourceDir + "\\" + findData.cFileName;
                std::string destPath = destDir + "\\" + findData.cFileName;

                if (!MoveFileA(sourcePath.c_str(), destPath.c_str())) {
                    success = false;
                    break;
                }
            }
        } while (FindNextFileA(hFind, &findData));

        FindClose(hFind);
        return success;
    }

    bool Extractor::extractInnerFolder(const std::string& zipPath, const std::string& outputPath) {
        try {
            std::string tempExtractPath = outputPath + "\\temp_extract";
            FileSystem::createDirectories(tempExtractPath);

            if (!extract(zipPath, tempExtractPath)) {
                FileSystem::removeAll(tempExtractPath);
                return false;
            }

            std::string firstFolder = findFirstSubDirectory(tempExtractPath);
            if (firstFolder.empty()) {
                FileSystem::removeAll(tempExtractPath);
                return false;
            }

            if (!moveDirectoryContents(firstFolder, outputPath)) {
                FileSystem::removeAll(tempExtractPath);
                return false;
            }

            FileSystem::removeAll(tempExtractPath);
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Dosya çýkartma hatasý: " << e.what() << std::endl;
            return false;
        }
    }

    bool Extractor::extract(const std::string& zipPath, const std::string& outputPath) {
        try {
            HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (FAILED(hr)) return false;

            struct CoUninitializeOnExit {
                ~CoUninitializeOnExit() { CoUninitialize(); }
            } coUninit;

            std::wstring wzipPath = stringToWString(zipPath);
            std::wstring woutputPath = stringToWString(outputPath);

            IShellDispatch* pShell = nullptr;
            hr = CoCreateInstance(CLSID_Shell, nullptr, CLSCTX_INPROC_SERVER,
                IID_IShellDispatch, (void**)&pShell);
            if (FAILED(hr)) return false;

            struct ReleaseShell {
                IShellDispatch* p;
                ~ReleaseShell() { if (p) p->Release(); }
            } releaseShell{ pShell };

            Folder* pZipFile = nullptr, * pDestination = nullptr;
            VARIANT vZipFile, vDestination;
            VariantInit(&vZipFile);
            VariantInit(&vDestination);

            vZipFile.vt = VT_BSTR;
            vZipFile.bstrVal = SysAllocString(wzipPath.c_str());
            vDestination.vt = VT_BSTR;
            vDestination.bstrVal = SysAllocString(woutputPath.c_str());

            struct FreeBSTR {
                VARIANT& vZip, & vDest;
                ~FreeBSTR() {
                    VariantClear(&vZip);
                    VariantClear(&vDest);
                }
            } freeBSTR{ vZipFile, vDestination };

            hr = pShell->NameSpace(vDestination, &pDestination);
            if (FAILED(hr)) return false;

            hr = pShell->NameSpace(vZipFile, &pZipFile);
            if (FAILED(hr)) return false;

            struct ReleaseFolders {
                Folder* zip, * dest;
                ~ReleaseFolders() {
                    if (zip) zip->Release();
                    if (dest) dest->Release();
                }
            } releaseFolders{ pZipFile, pDestination };

            if (pZipFile && pDestination) {
                FolderItems* pItems = nullptr;
                hr = pZipFile->Items(&pItems);
                if (FAILED(hr)) return false;

                struct ReleaseFolderItems {
                    FolderItems* p;
                    ~ReleaseFolderItems() { if (p) p->Release(); }
                } releaseItems{ pItems };

                VARIANT options;
                VariantInit(&options);
                options.vt = VT_I4;
                options.lVal = FOF_NO_UI | FOF_NOCONFIRMATION | FOF_SILENT;

                VARIANT vItems;
                VariantInit(&vItems);
                vItems.vt = VT_DISPATCH;
                vItems.pdispVal = pItems;

                hr = pDestination->CopyHere(vItems, options);
                if (FAILED(hr)) return false;

                Sleep(1000);
                return true;
            }

            return false;
        }
        catch (const std::exception& e) {
            std::cerr << "Dosya çýkartma hatasý: " << e.what() << std::endl;
            return false;
        }
    }
}