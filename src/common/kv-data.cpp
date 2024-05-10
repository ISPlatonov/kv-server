#include <iostream>
#include <fstream>
#include "kv-data.hpp"


KVData::KVData(boost::asio::io_service& io_service, bool save_async, std::string path) : timer(io_service), strand(io_service.get_executor())
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
            data.insert({key, KVStruct(value)});
        }

    }
    else
    {
        std::cout << "File does not exist" << std::endl;
    }
    if (!save_async)
        return;
    // start timer
    timer.expires_after(std::chrono::seconds(5));
    timer.async_wait(boost::bind(&KVData::save_to_file_handler, this, boost::asio::placeholders::error));
    timer.async_wait(boost::bind(&KVData::print_statistics, this));
}


std::string KVData::get(std::string key)
{
    auto it = data.find(key);
    if (it != data.end())
    {
        return it->second.get_value();
    }
    throw std::runtime_error("Key not found");
}


void KVData::set(std::string key, std::string value)
{
    data.insert({key, KVStruct(value)});
}


void KVData::save_to_file()
{
    std::ofstream file(this->path.string());
    for (auto &pair : data)
    {
        file << pair.first << " " << pair.second.get_value(false) << std::endl;
    }
    file.close();
}


void KVData::save_to_file_handler(const boost::system::error_code &ec)
{
    if (!ec)
    {
        save_to_file();
        timer.expires_after(timer.expires_from_now() + std::chrono::seconds(5));
        timer.async_wait(boost::bind(&KVData::save_to_file_handler, this, boost::asio::placeholders::error));
        timer.async_wait(boost::bind(&KVData::print_statistics, this));
    }
    else
    {
        std::cout << "Error: " << ec.message() << std::endl;
    }
}


void KVData::print_statistics()
{
    for (auto& pair : data)
    {
        std::cout << "Key: " << pair.first << "\n\tValue: " << pair.second.get_value(false) << "\n\tRead count: " << pair.second.get_read_count() << "\n\tWrite count: " << pair.second.get_write_count() << "\n\tRead count last 5s: " << pair.second.get_read_count_last_5s() << "\n\tWrite count last 5s: " << pair.second.get_write_count_last_5s() << std::endl;
    }
}
