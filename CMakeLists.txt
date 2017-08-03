cmake_minimum_required(VERSION 3.7)

project(todoom)

find_library(SQLITE_LIBRARY sqlite3)
find_library(LIBEVENT_LIBRARY event)

file(GLOB_RECURSE SOURCES "src/*")
file(GLOB_RECURSE HEADERS "include/*")

include_directories(include)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -lcurl")

add_executable(todoom ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${SQLITE_LIBRARY} ${LIBEVENT_LIBRARY} zip pthread stdc++fs)
