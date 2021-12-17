include(CMakePrintHelpers)

# Build the simplestrutils.
add_library(
    lib_simplestrutils STATIC
    "src/simplestrutils.cpp"
)

message(DEBUG "Variable CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# Usage requirements.
target_include_directories(
    lib_simplestrutils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

cmake_print_properties(
    TARGETS lib_simplestrutils
    PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
)

# Install the libraries and header files.
install(TARGETS lib_simplestrutils DESTINATION lib)
install(DIRECTORY "include/simplestrutils" DESTINATION include)
