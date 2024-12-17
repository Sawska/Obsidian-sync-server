cmake_minimum_required(VERSION 3.10)
project(Obsidian_sync)

set(CMAKE_CXX_STANDARD 14)

include_directories(
    "/usr/local/include"
    "/opt/homebrew/include"
)

add_executable(Obsidian_sync 
    Server.cpp Server.h 
    ServerFile.cpp ServerFile.h 
    ClientFile.cpp ClientFile.h 
    SyncFileReader.h SyncFileReader.cpp main.cpp
)
