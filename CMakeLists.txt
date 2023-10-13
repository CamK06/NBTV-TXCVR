cmake_minimum_required(VERSION 3.5)
project(NBTV)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_INSTALL_PREFIX "/usr/local")
else()
    set(CMAKE_INSTALL_PREFIX "/usr")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS src/dialogs)

find_package(SndFile REQUIRED)
find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(src)

file(GLOB SOURCES "src/*.cpp" "src/**/*.cpp")

add_executable(nbtv ${SOURCES})
target_link_libraries(nbtv PRIVATE SndFile::sndfile Qt5::Widgets stdc++fs)