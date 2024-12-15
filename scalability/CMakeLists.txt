cmake_minimum_required(VERSION 3.10)
project(MyBenchmarkProject)


include_directories(${PROJECT_SOURCE_DIR}/include)


# Specify C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add Google Benchmark
find_package(benchmark REQUIRED)

# Add your source files
add_executable(my_benchmark Example.cpp)

# Link Google Benchmark
target_link_libraries(my_benchmark PRIVATE benchmark::benchmark)
