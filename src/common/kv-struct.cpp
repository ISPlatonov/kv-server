#include "kv-struct.hpp"
#include <algorithm>
#include <iostream>
#include <mutex>


std::string KVStruct::get_value(bool count_as_read)
{
    if (count_as_read)
    {
        ++read_count;
        {
            std::lock_guard<std::mutex> lock(read_mtx);
            read_times.push_back(std::chrono::system_clock::now());
        }
    }
    return value;
}


void KVStruct::set_value(std::string value_)
{
    ++write_count;
    {
        std::lock_guard<std::mutex> lock(write_mtx);
        value = value_;
        write_times.push_back(std::chrono::system_clock::now());
    }
}


size_t KVStruct::get_read_count_last_5s()
{
    // lock the mutex
    std::lock_guard<std::mutex> lock(read_mtx);
    auto now = std::chrono::system_clock::now();
    // delete old times
    std::cout << "read_times.size() = " << read_times.size() << std::endl;
    read_times.erase(std::remove_if(read_times.begin(), read_times.end(), [&](auto &time) { return now - time > std::chrono::seconds(5); }), read_times.end());
    std::cout << "read_times.size() = " << read_times.size() << std::endl;
    return read_times.size();
}


size_t KVStruct::get_write_count_last_5s()
{
    std::lock_guard<std::mutex> lock(write_mtx);
    auto now = std::chrono::system_clock::now();
    // delete old times
    std::cout << "write_times.size() = " << write_times.size() << std::endl;
    write_times.erase(std::remove_if(write_times.begin(), write_times.end(), [&](auto &time) { return now - time > std::chrono::seconds(5); }), write_times.end());
    std::cout << "write_times.size() = " << write_times.size() << std::endl;
    return write_times.size();
}
