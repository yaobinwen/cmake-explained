cmake_minimum_required(VERSION 3.21)

# Set the project name and version
project(cmake-commands-project-03 VERSION 1.0)

add_subdirectory(sub1)

message(STATUS "PROJECT_NAME = ${PROJECT_NAME}")
