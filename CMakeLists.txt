cmake_minimum_required(VERSION 3.20)
set(CMAKE_C_STANDARD 11)

# To enable Debug symbols in the executable, uncomment this line
#set(CMAKE_BUILD_TYPE "Debug")

project(TerDimension)

# /build/main
add_executable(main src/main.c)

# Linux maths
target_link_libraries(main m)

# Uncomment this line if you have SDL installed system-wide
#target_link_libraries(main SDL)

# For multi-threading, not implemented yet
#target_link_libraries(main pthread)
