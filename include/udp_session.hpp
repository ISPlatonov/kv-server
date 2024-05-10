#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>


class udp_server;


struct udp_session : boost::enable_shared_from_this<udp_session>
{
    udp_session(udp_server* server) : server_(server) {}

    void handle_request(const boost::system::error_code& error);

    inline void handle_sent(const boost::system::error_code& ec, std::size_t) {
        // here response has been sent
        if (ec) {
            std::cout << "Error sending response to " << remote_endpoint_ << ": " << ec.message() << "\n";
        }
    }

    boost::asio::ip::udp::endpoint remote_endpoint_;
    boost::array<char, 128> recv_buffer_;
    std::string message;
    udp_server* server_;
};