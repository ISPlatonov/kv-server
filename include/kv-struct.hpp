#pragma once

#include <string>
#include <vector>
#include <chrono>


/**
 * @brief Key-Value data structure
 * 
 * This class is used to store key-value pairs with statistics of how many times each key was accessed for read and write.
 * It stores statistics for the whole lifetime as well as for the last 5 seconds.
 * 
 */
class KVStruct
{
public:
    KVStruct(std::string value_) : value(value_), write_times({std::chrono::system_clock::now()}) {}
    std::string get_value(bool count_as_read = true);
    void set_value(std::string value);
    inline size_t get_read_count() { return read_count; }
    inline size_t get_write_count() { return write_count; }
    size_t get_read_count_last_5s();
    size_t get_write_count_last_5s();


private:
    std::string value;
    size_t read_count = 0, write_count = 1;
    std::vector<std::chrono::time_point<std::chrono::system_clock>> read_times, write_times;
};
