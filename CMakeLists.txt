cmake_minimum_required(VERSION 3.10) # Or a newer version
project(SATsolver)

set(CMAKE_CXX_STANDARD 17) # Or a newer standard

# Add executable
add_executable(SATsolver src/main.cpp) # add the main file, and any other cpp files later.

# Add include directories (if needed)
# target_include_directories(SATsolver PUBLIC include)
