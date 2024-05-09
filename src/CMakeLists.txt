add_executable(kv-server kv-server.cpp kv-data.cpp)
target_link_libraries(kv-server Boost::asio Boost::thread Boost::filesystem)

add_executable(kv-client kv-client.cpp)
target_link_libraries(kv-client Boost::asio)
install(TARGETS kv-server kv-client DESTINATION ${CMAKE_SOURCE_DIR}/bin)