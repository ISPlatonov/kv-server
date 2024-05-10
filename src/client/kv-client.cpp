#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <string>
#include <unordered_map>
#include "kv-data.hpp"
#include <random>

using boost::asio::ip::udp;


std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}


std::string send_request(boost::asio::io_context &io_context, udp::endpoint receiver_endpoint, const std::string& request)
{
    try
    {
        udp::socket socket(io_context);
        socket.open(udp::v4());

        socket.send_to(boost::asio::buffer(request), receiver_endpoint);

        boost::array<char, 128> recv_buf;
        udp::endpoint sender_endpoint;
        size_t len = socket.receive_from(
            boost::asio::buffer(recv_buf), sender_endpoint);

        return std::string(recv_buf.begin(), len);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return "";
    }
}


int main(int argc, char *argv[])
{
    // set random seed
    srand(time(0));
    if (argc != 2)
    {
        std::cerr << "Usage: client <host>" << std::endl;
        return 1;
    }

    boost::asio::io_context io_context;

    udp::resolver resolver(io_context);
    udp::endpoint receiver_endpoint =
        *resolver.resolve(udp::v4(), argv[1], "daytime").begin();
    
    KVData kvdata;

    for (size_t i = 0; i < 10000; ++i)
    {
        switch (i % 100)
        {
        case 0: {
            // set request
            auto key = gen_random(5);
            auto value = gen_random(5);
            std::string request = "set " + key + " " + value;
            kvdata.set(key, value);
            std::string response = send_request(io_context, receiver_endpoint, request);
            assert(response == "OK");
            // std::cout << "Request: " << request << " Response: " << response << std::endl;
        }
            break;
        default: {
            // get request
            std::string key = kvdata.get_random_key();
            std::string request = "get " + key;
            std::string response = send_request(io_context, receiver_endpoint, request);
            assert(response == kvdata.get(key));
            // std::cout << "Request: " << request << " Response: " << response << std::endl;
        }
            break;
        }
    }

    return 0;
}