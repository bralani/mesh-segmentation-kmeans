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

# Enable coverage flags
option(ENABLE_COVERAGE "Enable code coverage" ON)
if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Add Mesh library
add_library(
    mesh_lib STATIC 
    ${CMAKE_SOURCE_DIR}/src/geometry/mesh/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/geometry/point/Point.cpp
)

# Define test executable
add_executable(test_kmeans
    ${CMAKE_SOURCE_DIR}/tests/test_main.cpp
    ${CMAKE_SOURCE_DIR}/tests/geometry/mesh/MeshTest.cpp
)

# Link against GTest and pthread
target_link_libraries(test_kmeans PRIVATE mesh_lib GTest::GTest pthread)

# Register the test
add_test(NAME KMeansTest COMMAND test_kmeans)

# 📌 Ensure `lcov` and `genhtml` are available
find_program(LCOV_EXECUTABLE lcov)
find_program(GENHTML_EXECUTABLE genhtml)

if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
    # 📌 Add custom target for coverage
    add_custom_target(coverage
        COMMAND ${LCOV_EXECUTABLE} --capture --directory . --output-file coverage.info
        COMMAND ${LCOV_EXECUTABLE} --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage_filtered.info
        COMMAND ${GENHTML_EXECUTABLE} coverage_filtered.info --output-directory coverage_report
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "📊 Generating code coverage report..."
    )
else()
    message(WARNING "⚠️ lcov/genhtml not found! Install them to generate coverage reports.")
endif()