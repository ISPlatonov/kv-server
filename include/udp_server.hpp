#pragma once

#include "udp_session.hpp"
#include <boost/asio.hpp>
#include "kv-data.hpp"


class udp_server
{
    typedef boost::shared_ptr<udp_session> shared_session;

public:
    udp_server(boost::asio::io_service& io_service);

private:
    void receive_session();
    void handle_receive(shared_session session, const boost::system::error_code& ec, std::size_t /*bytes_transferred*/);
    void enqueue_response(shared_session const& session);
    void enqueue_response_strand(shared_session const& session);

    boost::asio::ip::udp::socket socket_;
    boost::asio::io_context::strand strand_;

    KVData kvdata;

    friend struct udp_session;
};