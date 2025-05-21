# Define the minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Define the project name and language
project(MainProject C)

# Set the C standard version
set(CMAKE_C_STANDARD 11)

# Set output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Use GLOB to find all .c files dynamically
file(GLOB SOURCE_FILES
     "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/Modules/*.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/*.c"
)

# Add an executable target
add_executable(main ${SOURCE_FILES})

# Specify include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Modules
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compilation options
target_compile_options(main PRIVATE -g -fdiagnostics-color=always)

# Link necessary libraries
target_link_libraries(main fftw3 m pthread hackrf) # activar hackrf después
