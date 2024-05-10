add_subdirectory(server)
add_subdirectory(client)

install(TARGETS kv-server kv-client DESTINATION ${CMAKE_SOURCE_DIR}/bin)
