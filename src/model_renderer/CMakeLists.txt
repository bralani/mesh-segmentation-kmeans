cmake_minimum_required(VERSION 3.22)
project(Mesh_Viewer)

set(CMAKE_CXX_STANDARD 20)

# Specify the correct paths to GLFW
set(GLFW_INCLUDE_DIR /usr/local/opt/glfw/include)
set(GLFW_LIBRARY /usr/local/opt/glfw/lib/libglfw.dylib)

set(Glad_DIR 3rdparty/glad/cmake)
set(GLFW_DIR 3rdparty/glfw/cmake)
set(glm_DIR 3rdparty/glm/cmake/glm)

# Find OpenGL and GLFW via CMake
find_package(OpenGL REQUIRED)
# find_package(Glad REQUIRED)
find_package(glm REQUIRED)

# Add glad source files to the project
set(GLAD_SRC 3rdparty/glad/src/glad.c)

# Add the glad include directory
include_directories(3rdparty/glad/include)

# Add other include directories
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${Glad_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIR})
include_directories(${glm_INCLUDE_DIRS})
include_directories(3rdparty/plog/include)
include_directories(3rdparty/obj-loader)
include_directories(3rdparty/stb)

# Include ImGui
set(IMGUI_DIR 3rdparty/imgui)
include_directories(${IMGUI_DIR})
include_directories(${IMGUI_DIR}/backends)

# Add ImGui source files
set(IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Add executable
add_executable(${PROJECT_NAME}
        ${GLAD_SRC}   # Use GLAD_SRC variable here instead of Glad_SOURCE
        ${IMGUI_SRC}
        3rdparty/stb/stb_image.h
        src/main.cpp
        src/shader.h
        src/shader.cpp
        src/material.h
        src/material.cpp
        src/mesh.h
        src/mesh.cpp
        src/camera.h
        src/camera.cpp
)

# Link libraries correctly
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARY})
# target_link_libraries(${PROJECT_NAME} PRIVATE glad)  # Link against glad
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)  # Link against glm

# Handle platform-specific definitions
if(WIN32)
        add_definitions(-DPLATFORM_WINDOWS)
elseif(APPLE)
        add_definitions(-DPLATFORM_MACOS)
endif()

# Copy GLFW DLL if necessary (Windows)
file(COPY ${GLFW_DLL} DESTINATION ${CMAKE_BINARY_DIR})
