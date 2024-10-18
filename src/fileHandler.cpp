#include "../include/fileHandler.h"

std::vector<std::string> FileHandler::getFiles(const std::string &dirPath) // replace with QDirIterator from QT
{
    WIN32_FIND_DATAW ffd;
    std::wstring path = std::wstring(dirPath.begin(), dirPath.end());
    HANDLE hFind = FindFirstFileW(path.c_str(), &ffd);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("FindFirstFile failed: " + GetLastError());
    }

    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            _tprintf(TEXT("  %ls   <DIR>\n"), ffd.cFileName);
        }
        else
        {
            _tprintf(TEXT("  %ls \n"), ffd.cFileName);
        }
    } while (FindNextFileW(hFind, &ffd) != 0);

    FindClose(hFind);
}