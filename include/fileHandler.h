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
#include <fstream>

class FileHandler
{
public:
    static std::string getFiles(const std::string &dirPath);
    static std::string readFromFile(const std::string &dirPath, const std::string &filename);
};

#endif /* FILE_HANDLER_H*/