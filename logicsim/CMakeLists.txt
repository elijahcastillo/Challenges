cmake_minimum_required(VERSION 3.10)
project(logicsim)

# Specify the C standard
set(CMAKE_C_STANDARD 99)

# Include FetchContent module
include(FetchContent)

# Fetch Raylib
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        master
)

# Make Raylib available
FetchContent_MakeAvailable(raylib)

# Add the executable
set(SOURCES src/main.c src/draggable_rect.c src/logic_gate.c src/utils.c src/truth_table.c )
add_executable(logicsim ${SOURCES})

# Include directories
target_include_directories(logicsim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link Raylib
target_link_libraries(logicsim raylib)
