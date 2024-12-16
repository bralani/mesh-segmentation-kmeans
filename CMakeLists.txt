cmake_minimum_required(VERSION 3.10)
project(KMeans)

# Add submodule for matplotlib-cpp (if this is still needed for your project)
add_subdirectory(include/matplotlib-cpp)

# Set the target architecture for macOS (arm64)
set(CMAKE_OSX_ARCHITECTURES arm64)

# Set C++ standard version (if required)
# set(CMAKE_CXX_STANDARD 17)

# Include csv-parser as a submodule
set(CSV_PARSER_DIR "${CMAKE_SOURCE_DIR}/include/csv-parser")
include_directories(${CSV_PARSER_DIR}/single_include)

# Create the executable for your project
add_executable(k_means src/main.cpp)

# Link the matplotlib-cpp library (if still required)
target_link_libraries(k_means PRIVATE matplotlib_cpp)

# Specify the output directory for the compiled executable
set_target_properties(k_means PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Include directories for matplotlib-cpp and csv-parser
target_include_directories(k_means PRIVATE include)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
message(STATUS "Python3 Executable: ${Python3_EXECUTABLE}")
message(STATUS "Python3 Library: ${Python3_LIBRARIES}")
message(STATUS "Python3 Include Directory: ${Python3_INCLUDE_DIRS}")

target_include_directories(k_means PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(k_means PRIVATE ${Python3_LIBRARIES})

#Openmp
find_package(OpenMP REQUIRED)
target_link_libraries(k_means PUBLIC OpenMP::OpenMP_CXX)
set(ENV{OMP_NESTED} TRUE)
set(ENV{OMP_NUM_THREADS} 8)
