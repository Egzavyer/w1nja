#include "../include/fileHandler.h"

std::vector<std::string> FileHandler::getFiles(const std::string &dirPath)
{
    std::vector<std::string> v;

    if (!std::filesystem::exists(dirPath))
    {
        std::cerr << "Error: Directory does not exist: " << dirPath << std::endl;
        return v;
    }

    try
    {
        for (const auto &entry : std::filesystem::directory_iterator(dirPath))
        {
            std::filesystem::path filename = entry.path().filename();
            std::string strFilename = filename.string();
            v.push_back(strFilename);
            std::cout << filename << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    return v;
}

void FileHandler::readFromFile(const std::string &dirPath, const std::string &filename, char buf[1024])
{
    std::cout << "Reading file " + dirPath + '/' + filename << std::endl;
    std::ifstream file(dirPath + '/' + filename, std::ios::binary);
    file.get(buf, 1023);
    std::cout << buf << std::endl;
}