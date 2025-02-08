cmake_minimum_required(VERSION 3.10)
project(KMeansTests)

set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_CXX_STANDARD 20)

enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(
    ${CMAKE_SOURCE_DIR}/src         
    ${CMAKE_SOURCE_DIR}/include     
    ${CMAKE_SOURCE_DIR}/third_party 
    ${CMAKE_SOURCE_DIR}/third_party/matplotlib-cpp
)

option(ENABLE_COVERAGE "Enable code coverage" ON)
if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

add_library(
    mesh_lib STATIC 
    ${CMAKE_SOURCE_DIR}/src/geometry/mesh/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/geometry/point/Point.cpp
)

# 🛠 Fix: Add the correct test file
add_executable(test_kmeans
    ${CMAKE_SOURCE_DIR}/tests/test_main.cpp
    ${CMAKE_SOURCE_DIR}/tests/geometry/mesh/MeshTest.cpp  # ✅ Correct file
)

target_link_libraries(test_kmeans PRIVATE mesh_lib GTest::GTest pthread)

# 🛠 Fix: Make sure CTest sees the tests
add_test(
    NAME KMeansTest 
    COMMAND test_kmeans
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)