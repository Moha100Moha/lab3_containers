cmake_minimum_required(VERSION 3.10)
project(cpp_containers LANGUAGES CXX)

# Use the C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build the executable from main.cpp
add_executable(containers src/main.cpp)

# Tell the compiler where to look for the .hpp files
target_include_directories(containers PRIVATE include)

# Enable useful warnings
if (MSVC)
    target_compile_options(containers PRIVATE /W4)
else()
    target_compile_options(containers PRIVATE -Wall -Wextra)
endif()