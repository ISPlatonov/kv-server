#pragma once

#include "kv-struct.hpp"
#include <string>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>


class KVData
{
public:
    /**
     * @brief Construct a new KVData object
     * 
     * @param path 
     */
    KVData(boost::asio::io_service&, bool save_async = false, std::string path = "data/config.txt");

    std::string get(std::string key);
    void set(std::string key, std::string value);
    std::string get_random_key();

private:
    // async save to file
    void save_to_file();
    void print_statistics();
    void save_to_file_handler(const boost::system::error_code &ec);

    boost::filesystem::path path;
    std::unordered_map<std::string, KVStruct> data;
    // add timer and assinchrounous rewrite to file
    boost::asio::steady_timer timer;
    boost::asio::steady_timer save_timer;
    boost::asio::strand<boost::asio::io_context::executor_type> strand;
    bool data_changed = false;
    constexpr static std::chrono::seconds save_duration = std::chrono::seconds(5);
};