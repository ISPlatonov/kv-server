file(GLOB COMMON_FILES common/*.cpp)
file(GLOB SERVER_FILES server/*.cpp)
file(GLOB CLIENT_FILES client/*.cpp)

add_executable(kv-client ${CLIENT_FILES} ${COMMON_FILES})
target_link_libraries(kv-client Boost::asio Boost::filesystem Boost::system)

add_executable(kv-server ${SERVER_FILES} ${COMMON_FILES})
target_link_libraries(kv-server Boost::asio Boost::thread Boost::filesystem)


install(TARGETS kv-server kv-client DESTINATION ${CMAKE_SOURCE_DIR}/bin)
