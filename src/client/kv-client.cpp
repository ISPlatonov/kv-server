#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

void send_request(boost::asio::io_context &io_context, udp::endpoint receiver_endpoint)
{
    try
    {
        udp::socket socket(io_context);
        socket.open(udp::v4());

        boost::array<char, 1> send_buf = {{0}};
        socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

        boost::array<char, 128> recv_buf;
        udp::endpoint sender_endpoint;
        size_t len = socket.receive_from(
            boost::asio::buffer(recv_buf), sender_endpoint);

        //std::cout.write(recv_buf.data(), len);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: client <host>" << std::endl;
        return 1;
    }

    boost::asio::io_context io_context;

    udp::resolver resolver(io_context);
    udp::endpoint receiver_endpoint =
        *resolver.resolve(udp::v4(), argv[1], "daytime").begin();

    while (true)
    {
        send_request(io_context, receiver_endpoint);
    }

    return 0;
}