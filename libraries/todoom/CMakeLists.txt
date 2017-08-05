cmake_minimum_required(VERSION 3.7)

project(todoom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_library(SQLITE_LIBRARY sqlite3)
find_library(TODOOM_LIBRARY pthread event curl zip stdc++fs)

file(GLOB_RECURSE SOURCES "src/*")
file(GLOB_RECURSE HEADERS "include/*")

include_directories(include)

add_library(Todoom SHARED  src/CloudControl.cpp src/HttpUtils.cpp include/CloudControl.h include/HttpUtils.h include/json.hpp include/picosha2.h)

#add_executable(todoom ${SOURCES} ${HEADERS})
#target_link_libraries(${PROJECT_NAME} ${TODOOM_LIBRARY})
