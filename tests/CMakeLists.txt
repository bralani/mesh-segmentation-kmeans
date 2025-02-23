# Specify the minimum required CMake version and project name
cmake_minimum_required(VERSION 3.10)
project(KMeansTests)

# Set the target architecture and C++ standard
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_CXX_STANDARD 17)

# Enable testing for CMake
enable_testing()

# CUDA settings: locate CUDA toolkit and set GPU architecture
find_package(CUDA)
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_50")

# Include third-party dependencies for the project
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/csv-parser/single_include)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/eigen)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/libigl/include)

# Add GoogleTest as a subdirectory for unit testing
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/googletest ${CMAKE_BINARY_DIR}/googletest)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Define a function to add KMeans tests as executables
function(add_kmeans_test exe_name)
    set(source_files ${ARGN}) # Collect all arguments into a list

    if(CUDA_FOUND AND MSVC)
        add_definitions(-DUSE_CUDA)
        cuda_add_executable(${exe_name} ${source_files}) 
        target_link_libraries(${exe_name} ${CUDA_LIBRARIES})
        set_target_properties(${exe_name} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
    else()
        add_executable(${exe_name} ${source_files})
    endif()

    # Link common libraries (OpenMP, GoogleTest)
    target_link_libraries(${exe_name} OpenMP::OpenMP_CXX gtest gtest_main)

    # Set compiler options for OpenMP
    target_compile_options(${exe_name} PRIVATE ${OpenMP_CXX_FLAGS})

    # Include project-specific directories for headers
    target_include_directories(${exe_name} PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/third_party ${CMAKE_SOURCE_DIR}/src)

    # Set the output directory for the executable
    set_target_properties(${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endfunction()

# OpenMP settings: find OpenMP package and set environment variables for threading
find_package(OpenMP REQUIRED)
set(ENV{OMP_NESTED} TRUE)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp:llvm /openmp:experimental")
endif()

# Collect source files for the project
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/geometry/*.cpp" "${CMAKE_SOURCE_DIR}/src/clustering/*.cpp")
file(GLOB_RECURSE CUDA_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cu")

# Add the test executable
add_kmeans_test(my_tests 
    ${CMAKE_SOURCE_DIR}/tests/test_main.cpp
    ${CMAKE_SOURCE_DIR}/tests/geometry/mesh/MeshTest.cpp
    ${CMAKE_SOURCE_DIR}/tests/geometry/metrics/MetricTest.cpp
    ${CMAKE_SOURCE_DIR}/tests/geometry/metrics/EuclideanMetricTest.cpp
    ${CMAKE_SOURCE_DIR}/tests/geometry/metrics/GeodesicHeatMetricTest.cpp
    ${CMAKE_SOURCE_DIR}/tests/geometry/kdtree/KDNodeTest.cpp
    ${CMAKE_SOURCE_DIR}/tests/geometry/kdtree/KDTreeTest.cpp
    ${CMAKE_SOURCE_DIR}/tests/clustering/KMeansTest.cpp
    ${CMAKE_SOURCE_DIR}/tests/clustering/CentroidInitializationMethods/CentroidInitMethodsTest.cpp
    ${CMAKE_SOURCE_DIR}/tests/clustering/CentroidInitializationMethods/KDEBaseTest.cpp
    ${SOURCES} 
    ${CUDA_SOURCES}
)

add_test(NAME MyTest COMMAND my_tests)
