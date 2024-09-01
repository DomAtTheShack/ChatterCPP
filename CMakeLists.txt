cmake_minimum_required(VERSION 3.29)
project(ChatterCPP)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED)

add_executable(ChatterCPP_Server Server/main.cpp
        Packet.cpp
        Packet.h
        Server/User.cpp
        Server/User.h)

add_executable(ChatterCPP_Client Client/main.cpp
        Packet.cpp
        Packet.h)

target_link_libraries(ChatterCPP_Client OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(ChatterCPP_Server OpenSSL::SSL OpenSSL::Crypto)

