cmake_minimum_required(VERSION 3.15)
project(V5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------
# ImGui setup
# -----------------------
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# -----------------------
# Dependencies
# -----------------------
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# ---- Add Matplot++ as a subdirectory ----
add_subdirectory(external/matplotplusplus)

# -----------------------
# Executable
# -----------------------
add_executable(my_program
    main.cpp
    data.cpp
    func.cpp
    Greeks.cpp
    ${IMGUI_SOURCES}
)

# -----------------------
# Includes
# -----------------------
target_include_directories(my_program PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# -----------------------
# Link everything
# -----------------------
target_link_libraries(my_program PRIVATE
    Matplot++::matplot
    glfw
    OpenGL::GL
    dl
    pthread
)

if(APPLE)
    target_link_libraries(my_program PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()
