cmake_minimum_required(VERSION 3.21)

# Set the project name and version
project(CMakeExplained_UsageReq_2 VERSION 1.0)

find_package("lib_simplestrutils")

# Build the executable that uses the `simplestrutils` library.
add_executable(
    main_using_lib
    "main.cpp"
)
target_link_libraries(
    main_using_lib
    lib_simplestrutils
)
