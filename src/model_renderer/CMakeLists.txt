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

# Define the paths
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/src/model_renderer/shader)
set(MODEL_DIR ${CMAKE_SOURCE_DIR}/resources/meshes/obj)

# Pass paths to C++ code
add_definitions(
    -DSHADER_DIR="${SHADER_DIR}"
    -DMODEL_DIR="${MODEL_DIR}"
)

# Define the input and output files for the configuration
configure_file(
    config/config.h.in  # Input file
    ${CMAKE_BINARY_DIR}/config.h           # Output file
    @ONLY                                     # Only replace the variables in the file
)

# If config.h is generated in the build directory, include it explicitly
if(EXISTS ${CMAKE_BINARY_DIR}/config.h)
    include_directories(${CMAKE_BINARY_DIR})
endif()

# Handle platform-specific definitions
if(WIN32)
        add_definitions(-DPLATFORM_WINDOWS)
elseif(APPLE)
        add_definitions(-DPLATFORM_MACOS)
endif()

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

# Copy GLFW DLL if necessary (Windows)
file(COPY ${GLFW_DLL} DESTINATION ${CMAKE_BINARY_DIR})



add_library(model_renderer STATIC 
        ${GLAD_SRC}
        ${IMGUI_SRC}
        ${THIRD_PARTY_DIR}/stb/stb_image.h
        render.cpp
        shader.cpp
        material.cpp
        mesh.cpp
        camera.cpp
)
target_link_libraries(model_renderer INTERFACE ${OPENGL_LIBRARIES})
target_link_libraries(model_renderer INTERFACE ${GLFW_LIBRARY})
target_link_libraries(model_renderer INTERFACE glm::glm) 
target_include_directories(model_renderer PUBLIC include/)
target_include_directories(model_renderer INTERFACE ${THIRD_PARTY_DIR}/glad/include)

# Add other include directories
target_include_directories(model_renderer INTERFACE ${OPENGL_INCLUDE_DIRS})
target_include_directories(model_renderer INTERFACE ${Glad_INCLUDE_DIRS})
target_include_directories(model_renderer INTERFACE ${GLFW_INCLUDE_DIR})
target_include_directories(model_renderer INTERFACE ${glm_INCLUDE_DIRS})
target_include_directories(model_renderer INTERFACE ${THIRD_PARTY_DIR}/plog/include)
target_include_directories(model_renderer INTERFACE ${THIRD_PARTY_DIR}/obj-loader)
target_include_directories(model_renderer INTERFACE ${THIRD_PARTY_DIR}/stb)
target_include_directories(model_renderer INTERFACE ${THIRD_PARTY_DIR}/glm)

# Include ImGui
target_include_directories(model_renderer INTERFACE ${IMGUI_DIR})
target_include_directories(model_renderer INTERFACE ${IMGUI_DIR}/backends)

