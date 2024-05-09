#pragma once

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
    KVData(std::string path);

    inline std::string get(std::string key) { return data.count(key) ? data[key] : ""; }
    inline void set(std::string key, std::string value) { data[key] = value; }

private:
    boost::filesystem::path path;
    std::unordered_map<std::string, std::string> data;
    // add timer and assinchrounous rewrite to file
    // boost::asio::steady_timer timer;
    // boost::asio::strand<boost::asio::io_context::executor_type> strand;
};