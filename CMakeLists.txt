cmake_minimum_required(VERSION 2.8)

project(chatting)

include_directories(
    /usr/include/boost
    /usr/include/openssl
    ${PROJECT_SOURCE_DIR}/client
    ${PROJECT_SOURCE_DIR}/server
    ${PROJECT_SOURCE_DIR}/include
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_subdirectory(client)
add_subdirectory(server)
