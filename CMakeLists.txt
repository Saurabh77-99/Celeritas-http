cmake_minimum_required(VERSION 3.10)
project(hft_http_server)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(server
    src/main.cpp
    src/http_server.cpp
    src/http.cpp
    src/router.cpp
)
