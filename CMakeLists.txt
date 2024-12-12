cmake_minimum_required(VERSION 3.10)
project(KMeans)

add_subdirectory(include/matplotlib-cpp)


set(CMAKE_OSX_ARCHITECTURES arm64)

add_executable(plot src/plot.cpp)
target_link_libraries(plot PRIVATE matplotlib_cpp)
set_target_properties(plot PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
message(STATUS "Python3 Executable: ${Python3_EXECUTABLE}")
message(STATUS "Python3 Library: ${Python3_LIBRARIES}")
message(STATUS "Python3 Include Directory: ${Python3_INCLUDE_DIRS}")

target_include_directories(plot PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(plot PRIVATE ${Python3_LIBRARIES})