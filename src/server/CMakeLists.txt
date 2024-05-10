add_executable(kv-server kv-server.cpp kv-data.cpp udp_server.cpp)
target_link_libraries(kv-server Boost::asio Boost::thread Boost::filesystem)
