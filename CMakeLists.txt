cmake_minimum_required(VERSION 3.14)
project(WildWestMG C)

set(CMAKE_C_STANDARD 11)

# Generates compile_commands.json for your Neovim LSP (clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

# Automatically fetch and build Raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.0 # Lock to a stable version
)
FetchContent_MakeAvailable(raylib)

# Add your source files here
add_executable(${PROJECT_NAME} src/main.c)

# Link Raylib to your game
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
