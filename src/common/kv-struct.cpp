#include "kv-struct.hpp"
#include <algorithm>


std::string KVStruct::get_value(bool count_as_read)
{
    if (count_as_read)
    {
        read_count++;
        read_times.push_back(std::chrono::system_clock::now());
    }
    return value;
}


void KVStruct::set_value(std::string value)
{
    write_count++;
    write_times.push_back(std::chrono::system_clock::now());
    this->value = value;
}


size_t KVStruct::get_read_count_last_5s()
{
    auto now = std::chrono::system_clock::now();
    // delete old times
    read_times.erase(std::remove_if(read_times.begin(), read_times.end(), [&](auto &time) { return now - time > std::chrono::seconds(5); }), read_times.end());
    return read_times.size();
}


size_t KVStruct::get_write_count_last_5s()
{
    auto now = std::chrono::system_clock::now();
    // delete old times
    write_times.erase(std::remove_if(write_times.begin(), write_times.end(), [&](auto &time) { return now - time > std::chrono::seconds(5); }), write_times.end());
    return write_times.size();
}
