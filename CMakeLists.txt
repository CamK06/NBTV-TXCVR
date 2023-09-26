cmake_minimum_required(VERSION 3.1)
project(NBTV)

file(GLOB SOURCES "src/*.cpp")

add_executable(nbtv ${SOURCES})
#target_link_libraries()