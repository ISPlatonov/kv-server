#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <atomic>
#include <mutex>


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
    KVStruct(std::string value_) : value(value_), read_count(0), write_count(0) {}
    std::string get_value(bool count_as_read = true);
    void set_value(std::string value);
    inline size_t get_read_count() { return read_count.load(); }
    inline size_t get_write_count() { return write_count.load(); }
    size_t get_read_count_last_5s();
    size_t get_write_count_last_5s();


private:
    std::string value;
    std::mutex read_mtx, write_mtx;
    std::atomic<size_t> read_count, write_count;
    std::vector<std::chrono::time_point<std::chrono::system_clock>> read_times, write_times;
};
