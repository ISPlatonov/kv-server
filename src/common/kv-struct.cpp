#include "kv-struct.hpp"
#include <algorithm>
#include <iostream>
#include <mutex>


std::string KVStruct::get_value(bool count_as_read)
{
    if (count_as_read)
    {
        ++read_count;
        ++read_count_lt;
    }
    return value;
}


void KVStruct::set_value(std::string value_)
{
    {
        std::lock_guard<std::mutex> lock(set_value_mtx);
        value = value_;
    }
    ++write_count;
    ++write_count_lt;
}


size_t KVStruct::get_read_count_lt()
{
    size_t count = read_count_lt.load();
    read_count_lt = 0;
    return count;
}


size_t KVStruct::get_write_count_lt()
{
    size_t count = write_count_lt.load();
    write_count_lt = 0;
    return count;
}
