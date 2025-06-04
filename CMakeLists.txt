# cmake_minimum_required(VERSION 3.10)
# project(InventoryApp)

# set(CMAKE_CXX_STANDARD 17)

# # Include directories
# include_directories(include sqlite src src/imgui src/imgui/backends)

# # Find SDL2
# find_package(SDL2 REQUIRED)
# include_directories(${SDL2_INCLUDE_DIRS})

# # ImGui source files
# file(GLOB IMGUI_SRC
#     "src/imgui/*.cpp"
#     "src/imgui/backends/imgui_impl_sdl2.cpp"
#     "src/imgui/backends/imgui_impl_opengl3.cpp"
# )

# # Build target
# add_executable(inventory-app
#     src/main.cpp
#     src/db.cpp
#     src/gui.cpp
#     sqlite/sqlite3.c
#     ${IMGUI_SRC}
# )

# # Link with SDL2 and OpenGL
# target_link_libraries(inventory-app ${SDL2_LIBRARIES} "-framework OpenGL")

cmake_minimum_required(VERSION 3.10)
project(InventoryApp)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include sqlite src src/imgui src/imgui/backends)

# Find SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})

# ImGui source files
file(GLOB IMGUI_SRC
    "src/imgui/*.cpp"
    "src/imgui/backends/imgui_impl_sdl2.cpp"
    "src/imgui/backends/imgui_impl_opengl3.cpp"
)

add_executable(inventory-app
    src/main.cpp
    src/db.cpp
    src/gui.cpp
    sqlite/sqlite3.c
    ${IMGUI_SRC}
)

# Link libraries and macOS frameworks
target_link_libraries(inventory-app
    ${SDL2_LIBRARIES}
    ${SDL2_STATIC_LIBRARIES}
    ${SDL2_LINK_LIBRARIES}
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)
