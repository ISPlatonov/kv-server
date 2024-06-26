﻿#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "udp_server.hpp"


int main()
{
    try {
        boost::asio::io_service io_service;
        udp_server server(io_service);
        // work to keep io_service running
        boost::asio::io_service::work work(io_service);

        boost::thread_group group;
        for (unsigned i = 0; i < boost::thread::hardware_concurrency(); ++i)
            group.create_thread(bind(&boost::asio::io_service::run, boost::ref(io_service)));

        group.join_all();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}