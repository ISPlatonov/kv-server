#include <iostream>
#include <fstream>
#include "kv-data.hpp"


KVData::KVData(std::string path)
{
    this->path = boost::filesystem::path(path);
    // read data from file
    if (boost::filesystem::exists(this->path))
    {
        std::ifstream file(this->path.string());
        std::string line;
        while (std::getline(file, line))
        {
            std::string key = line.substr(0, line.find(" "));
            std::string value = line.substr(line.find(" ") + 1);
            data[key] = value;
        }

    }
    else
    {
        std::cout << "File does not exist" << std::endl;
    }
    // for (auto &pair : data)
    // {
    //     std::cout << pair.first << " " << pair.second << std::endl;
    // }
}


std::string KVData::get(std::string key)
{
    if (data.find(key) != data.end())
    {
        return data[key];
    }
    return "Key not found";
}


void KVData::set(std::string key, std::string value)
{
    data[key] = value;
    // std::ofstream file(this->path.string());
    // for (auto &pair : data)
    // {
    //     file << pair.first << " " << pair.second << std::endl;
    // }
    // file.close();
}