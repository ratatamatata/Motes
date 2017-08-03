cmake_minimum_required(VERSION 3.7)
project(Motes)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# Find includes in corresponding build directories

# declaring files in your project
file(GLOB_RECURSE SOURCES "./src/*.cpp")
file(GLOB_RECURSE MOC_HEADERS "./src/*.h")
file(GLOB_RECURSE UIS "./src/ui/*.ui")

# Next lines needed for building all Qt projects
# Find the QtWidgets library
find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Gui)
find_package(Qt5WebEngineWidgets)

include_directories(libraries/todoom/include)
add_subdirectory(libraries/todoom/)

# compile
add_executable( ${PROJECT_NAME} ${SOURCES} ${MOC_HEADERS} ${UIS})
# build it (link libraries)
target_link_libraries( ${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui Qt5::WebEngineWidgets Todoom pthread event curl zip stdc++fs)
