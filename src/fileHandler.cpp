#include "../include/fileHandler.h"

std::string FileHandler::getFiles(const std::string &dirPath)
{
    std::string files;

    if (!std::filesystem::exists(dirPath))
    {
        std::cerr << "Error: Directory does not exist: " << dirPath << std::endl;
        return files;
    }

    try
    {
        for (const auto &entry : std::filesystem::directory_iterator(dirPath))
        {
            std::filesystem::path filename = entry.path().filename();
            std::string strFilename = filename.string();
            files += strFilename + '\n';
            std::cout << filename << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    return files;
}

std::string FileHandler::readFromFile(const std::string &dirPath, const std::string &filename)
{
    std::string fullPath = dirPath + '/' + filename;
    std::cout << "Reading file: " << fullPath << std::endl;

    std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
    if (!file)
    {
        throw std::runtime_error("Cannot open file: " + fullPath);
    }

    auto size = file.tellg();
    if (size == -1)
    {
        throw std::runtime_error("Could not determine file size");
    }

    std::string str(size, '\0');
    file.seekg(0);
    if (!file.read(&str[0], size))
    {
        throw std::runtime_error("Failed to read file contents");
    }

    return str;
}