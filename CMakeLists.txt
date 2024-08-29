cmake_minimum_required(VERSION 3.29)
project(ChatterCPP)

set(CMAKE_CXX_STANDARD 20)

add_executable(ChatterCPP_Server Server/main.cpp
        Packet.cpp
        Packet.h)

add_executable(ChatterCPP_Client Client/main.cpp
        Packet.cpp
        Packet.h)

