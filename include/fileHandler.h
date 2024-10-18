#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <cstring>
#include <vector>
#include <filesystem>

class FileHandler
{
public:
    static std::vector<std::string> getFiles(const std::string &dirPath);
};

#endif /* FILE_HANDLER_H*/