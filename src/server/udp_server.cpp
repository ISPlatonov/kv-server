#include "udp_server.hpp"
#include <boost/asio.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind/bind.hpp>
#include "daytime_func.hpp"
#include <algorithm>
#include <iostream>

using boost::asio::ip::udp;

typedef boost::shared_ptr<udp_session> shared_session;


udp_server::udp_server(boost::asio::io_service& io_service) : socket_(io_service, udp::endpoint(udp::v4(), 13)), strand_(io_service)
{
    receive_session();
}


void udp_server::receive_session()
{
    // our session to hold the buffer + endpoint
    auto session = boost::make_shared<udp_session>(this);

    socket_.async_receive_from(
            boost::asio::buffer(session->recv_buffer_),
            session->remote_endpoint_,
            strand_.wrap(
                boost::bind(&udp_server::handle_receive, this,
                    session, // keep-alive of buffer/endpoint
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred)));
}


void udp_server::handle_receive(shared_session session, const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    std::string message_rcv = std::string(session->recv_buffer_.begin(), bytes_transferred);
    // parse 2 arguments
    auto arg_count = std::count(message_rcv.begin(), message_rcv.end(), ' ') + 1;
    switch (arg_count) {
        case 2: {
            std::string command = message_rcv.substr(0, message_rcv.find(' '));
            std::string key = message_rcv.substr(message_rcv.find(' ') + 1);
            if (command == "get") {
                std::string response = kvdata.get(key);
                session->message = response;
            }
        }
            break;
        case 3: {
            std::string command = message_rcv.substr(0, message_rcv.find(' '));
            std::string key = message_rcv.substr(command.size() + 1, message_rcv.find(' ', command.size() + 1) - command.size() - 1);
            std::string value = message_rcv.substr(command.size() + 1 + key.size() + 1, message_rcv.find(' ', command.size() + 1 + key.size() + 1) - command.size() - 1 - key.size() - 1);
            if (command == "set") {
                kvdata.set(key, value);
                session->message = "OK";
            }
        }
            break;
        default:
            std::cout << "Invalid request: " << message_rcv << "\n";
            receive_session();
            break;
    }

    // now, handle the current session on any available pool thread
    boost::asio::post(socket_.get_executor(), boost::bind(&udp_session::handle_request, session, ec));

    // immediately accept new datagrams
    receive_session();
}

    
void udp_server::enqueue_response(shared_session const& session)
{
    // async_send_to() is not thread-safe, so use a strand.
    boost::asio::post(socket_.get_executor(),
        strand_.wrap(boost::bind(&udp_server::enqueue_response_strand, this, session)));
}

    
void udp_server::enqueue_response_strand(shared_session const& session)
{
    socket_.async_send_to(boost::asio::buffer(session->message), session->remote_endpoint_,
            strand_.wrap(boost::bind(&udp_session::handle_sent,
                    session, // keep-alive of buffer/endpoint
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred)));
}


void udp_session::handle_request(const boost::system::error_code& error)
{
    if (!error || error == boost::asio::error::message_size)
    {
        // let the server coordinate actual IO
        server_->enqueue_response(shared_from_this());
    }
}
