cmake_minimum_required(VERSION 3.21)

# Set the project name and version
project(cmake-modules-CheckLanguage VERSION 1.0)

include(CheckLanguage)

check_language(Fortran)
if(CMAKE_Fortran_COMPILER)
    message(STATUS "Fortran can be enabled")
else()
    message(STATUS "No Fortran support")
endif()
