cmake_minimum_required (VERSION 3.16)
project(kv-server)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(BOOST_ENABLE_CMAKE ON)

set(BOOST_INCLUDE_LIBRARIES asio thread filesystem)
set(BOOST_ENABLE_CMAKE ON)
include (FetchContent)
FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.7z
  USES_TERMINAL_DOWNLOAD TRUE
  DOWNLOAD_NO_EXTRACT FALSE
)
FetchContent_MakeAvailable(Boost)

include_directories(include)
add_subdirectory (src)

# copy data directory to build directory
file(COPY data DESTINATION ${CMAKE_BINARY_DIR})
file(COPY data DESTINATION ${CMAKE_BINARY_DIR}/bin)
