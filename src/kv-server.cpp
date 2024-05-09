#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::udp;
using boost::system::error_code;

static std::string make_daytime_string()
{
    return boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time());
}

class udp_server; // forward declaration

struct udp_session : boost::enable_shared_from_this<udp_session> {

    udp_session(udp_server* server) : server_(server) {}

    void handle_request(const error_code& error);

    void handle_sent(const error_code& ec, std::size_t) {
        // here response has been sent
        if (ec) {
            std::cout << "Error sending response to " << remote_endpoint_ << ": " << ec.message() << "\n";
        }
    }

    udp::endpoint remote_endpoint_;
    boost::array<char, 100> recv_buffer_;
    std::string message;
    udp_server* server_;
};

class udp_server
{
    typedef boost::shared_ptr<udp_session> shared_session;
  public:
    udp_server(boost::asio::io_service& io_service)
        : socket_(io_service, udp::endpoint(udp::v4(), 13)), 
          strand_(io_service)
    {
        receive_session();
    }

  private:
    void receive_session()
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

    void handle_receive(shared_session session, const error_code& ec, std::size_t /*bytes_transferred*/) {
        // now, handle the current session on any available pool thread
        boost::asio::post(socket_.get_executor(), boost::bind(&udp_session::handle_request, session, ec));

        // immediately accept new datagrams
        receive_session();
    }

    void enqueue_response(shared_session const& session) {
        // async_send_to() is not thread-safe, so use a strand.
        boost::asio::post(socket_.get_executor(),
            strand_.wrap(boost::bind(&udp_server::enqueue_response_strand, this, session)));
    }

    void enqueue_response_strand(shared_session const& session) {
        socket_.async_send_to(boost::asio::buffer(session->message), session->remote_endpoint_,
                strand_.wrap(boost::bind(&udp_session::handle_sent,
                        session, // keep-alive of buffer/endpoint
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)));
    }

    udp::socket socket_;
    boost::asio::io_context::strand strand_;

    friend struct udp_session;
};

void udp_session::handle_request(const error_code& error)
{
    if (!error || error == boost::asio::error::message_size)
    {
        message = make_daytime_string(); // let's assume this might be slow
        message += "\n";

        // let the server coordinate actual IO
        server_->enqueue_response(shared_from_this());
    }
}

int main()
{
    try {
        boost::asio::io_service io_service;
        udp_server server(io_service);

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