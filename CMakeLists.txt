cmake_minimum_required(VERSION 3.10)
project(KMeans)

add_subdirectory(include/matplotlib-cpp)


add_executable(plot src/plot.cpp)
target_link_libraries(plot PRIVATE matplotlib_cpp)
set_target_properties(plot PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

