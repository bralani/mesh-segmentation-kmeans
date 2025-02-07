cmake_minimum_required(VERSION 3.10)
project(KMeansTests)

# Set the target architecture for macOS (arm64)
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_CXX_STANDARD 20)

enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Ensure the main project's include directories are available
include_directories(
    ${CMAKE_SOURCE_DIR}/src         # Includes src/, where clustering should be
    ${CMAKE_SOURCE_DIR}/include     # Includes include/, if header files are stored there
    ${CMAKE_SOURCE_DIR}/third_party # Includes third-party dependencies
    ${CMAKE_SOURCE_DIR}/third_party/matplotlib-cpp
)

# Add Mesh library
add_library(
    mesh_lib STATIC 
    ${CMAKE_SOURCE_DIR}/src/geometry/mesh/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/geometry/point/Point.cpp
)

# Define test executable
add_executable(test_kmeans ${CMAKE_SOURCE_DIR}/tests/geometry/mesh/Mesh.cpp)

# Link against GTest and pthread
target_link_libraries(test_kmeans PRIVATE mesh_lib GTest::GTest GTest::Main pthread)

# Register the test
add_test(NAME KMeansTest COMMAND test_kmeans)
