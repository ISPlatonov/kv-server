#include "kv-data.hpp"
#include <iostream>
#include <fstream>
#include <boost/bind.hpp>


KVData::KVData(boost::asio::io_service& io_service, bool save_async, std::string path) : timer(io_service), save_timer(io_service), strand(io_service.get_executor())
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
            data.emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(value));
        }

    }
    else
    {
        std::cout << "File does not exist" << std::endl;
    }
    if (!save_async)
        return;
    // start timer
    //timer.expires_after(std::chrono::seconds(5));
    save_timer.expires_after(std::chrono::seconds(5));
    //timer.async_wait(boost::bind(&KVData::print_statistics, this));
    save_timer.async_wait(boost::asio::bind_executor(strand, boost::bind(&KVData::save_to_file_handler, this, boost::asio::placeholders::error)));
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
    // use strand to protect data
    boost::asio::post(strand, [this, key, value]() {
        auto it = data.find(key);
        if (it != data.end())
        {
            it->second.set_value(value);
        }
        else
        {
            data.emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(value));
        }
        data_changed = true;
    });
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
        print_statistics();
        if (data_changed)
            save_to_file();
        data_changed = false;
        save_timer.expires_after(std::chrono::seconds(5));
        save_timer.async_wait(boost::bind(&KVData::save_to_file_handler, this, boost::asio::placeholders::error));
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
    //timer.expires_after(std::chrono::seconds(5));
    //timer.async_wait(boost::bind(&KVData::print_statistics, this));
}


std::string KVData::get_random_key() {
    auto it = data.begin();
    auto adv = std::rand() % data.size();
    std::advance(it, adv);
    return it->first;
}
