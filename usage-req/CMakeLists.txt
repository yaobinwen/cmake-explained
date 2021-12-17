cmake_minimum_required(VERSION 3.21)

include(CMakePrintHelpers)

# Set the project name and version
project(CMakeExplained_UsageReq VERSION 1.0)

message(DEBUG "Variable CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(simplestrutils)

cmake_print_properties(
    TARGETS lib_simplestrutils
    PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
)

# Build the executable that doesn't use the `simplestrutils` library at all.
add_executable(
    main_not_using
    "main-not-using.cpp"
)

# Build the executable that uses the `simplestrutils` library.
add_executable(
    main_only_including_header
    "main-only-including-header.cpp"
)
# NOTE(ywen): The command `target_link_libraries` seems to introduce the
# usage requirements of the mentioned libraries. Without this command, there
# will be the compile error "simplestrutils/simplestrutils.hpp: No such file or
# directory".
target_link_libraries(
    main_only_including_header
    lib_simplestrutils
)

# Build the executable that uses the `simplestrutils` library.
add_executable(
    main_using_lib
    "main-using-lib.cpp"
)
target_link_libraries(
    main_using_lib
    lib_simplestrutils
)

# Install them.
install(PROGRAMS $<TARGET_FILE:main_not_using> TYPE BIN)
install(PROGRAMS $<TARGET_FILE:main_only_including_header> TYPE BIN)
install(PROGRAMS $<TARGET_FILE:main_using_lib> TYPE BIN)
