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

std::string FileHandler::readFromFile(const std::string &dirPath, const std::string &filename)
{
    std::cout << "Reading file " + dirPath + '/' + filename << std::endl;
    std::ifstream file(dirPath + '/' + filename, std::ios::binary | std::ios::ate);
    auto size = file.tellg();
    std::string str(size, '\0');
    file.seekg(0);
    file.read(&str[0], size);
    // std::cout << str << '\n';
    return str;
}