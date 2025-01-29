cmake_minimum_required(VERSION 3.22)
project(Mesh_Viewer)

set(CMAKE_CXX_STANDARD 20)

# Specify the correct paths to GLFW
set(GLFW_INCLUDE_DIR /opt/homebrew/opt/glfw/include)
set(GLFW_LIBRARY /opt/homebrew/opt/glfw/lib/libglfw.dylib)

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party)

set(Glad_DIR ${THIRD_PARTY_DIR}/glad/cmake)
set(GLFW_DIR ${THIRD_PARTY_DIR}/glfw/cmake)
set(glm_DIR ${THIRD_PARTY_DIR}/glm/cmake/glm)

# Find OpenGL and GLFW via CMake
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

# Add glad source files to the project
set(GLAD_SRC ${THIRD_PARTY_DIR}/glad/src/glad.c)

# Add the glad include directory
include_directories(${THIRD_PARTY_DIR}/glad/include)

# Add other include directories
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${Glad_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIR})
include_directories(${glm_INCLUDE_DIRS})
include_directories(${THIRD_PARTY_DIR}/plog/include)
include_directories(${THIRD_PARTY_DIR}/obj-loader)
include_directories(${THIRD_PARTY_DIR}/stb)
include_directories(${THIRD_PARTY_DIR}/glm)

# Include ImGui
set(IMGUI_DIR ${THIRD_PARTY_DIR}/imgui)
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
        ${GLAD_SRC}
        ${IMGUI_SRC}
        ${THIRD_PARTY_DIR}/stb/stb_image.h
        main.cpp
        shader.h
        shader.cpp
        material.h
        material.cpp
        mesh.h
        mesh.cpp
        camera.h
        camera.cpp
)

# Link libraries correctly
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm) 

# Handle platform-specific definitions
if(WIN32)
        add_definitions(-DPLATFORM_WINDOWS)
elseif(APPLE)
        add_definitions(-DPLATFORM_MACOS)
endif()

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

# Copy GLFW DLL if necessary (Windows)
file(COPY ${GLFW_DLL} DESTINATION ${CMAKE_BINARY_DIR})
