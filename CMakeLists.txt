cmake_minimum_required(VERSION 3.20)
set(CMAKE_C_STANDARD 11)

# To enable Debug symbols in the executable, uncomment this line
#set(CMAKE_BUILD_TYPE "Debug")

# To enable profiling
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")

# To maximise performance at the cost of compilation speed
#set(CMAKE_BUILD_TYPE "Release")

project(TerDimension)

# Build the TerDimension library
add_library(TD STATIC
	src/TD/td_texture.c
	src/TD/td_time.c
	src/TD/td_obj.c
	src/TD/td_render.c
	src/TD/td_input.c
	src/TD/td_math.c
	src/TD/td_raster.c
	src/TD/terdimension.c
)

# Linux math library
target_link_libraries(TD m)

# ./build/main
add_executable(main src/main.c)

# Link the TerDimension library
target_link_libraries(main PRIVATE TD)
