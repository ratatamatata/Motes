cmake_minimum_required(VERSION 3.1)

project(todoom)

find_library(PTHREAD_LIBRARY pthread)
find_library(RT_LIBRARY rt)
find_library(BOOST_LIBRARY boost_system)
find_library(SQLITE_LIBRARY sqlite3)

file(GLOB_RECURSE SOURCES "src/*")
file(GLOB_RECURSE HEADERS "include/*")

include_directories(include)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -lcurl")

add_executable(todoom ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIBRARY} ${RT_LIBRARY} ${BOOST_LIBRARY} ${SQLITE_LIBRARY} ${CURL_LIBRARY})
