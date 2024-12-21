cmake_minimum_required(VERSION 3.10)
project(Obsidian_sync)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    include_directories("/usr/local/include")
    include_directories("/usr/include")
    include_directories("/usr/include/asio")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include_directories("/opt/homebrew/include")
    include_directories("/usr/local/include")
endif()

include_directories("/home/alexander/Документи/Obsidian-sync-server/Crow/include")

add_executable(Obsidian_sync
    Server.cpp Server.h
    ServerFile.cpp ServerFile.h
    ClientFile.cpp ClientFile.h
    SyncFileReader.h SyncFileReader.cpp
    main.cpp
)
