cmake_minimum_required(VERSION 3.10)
project(KMeans)

# Add submodule for matplotlib-cpp (if this is still needed for your project)
add_subdirectory(include/matplotlib-cpp)

# Set the target architecture for macOS (arm64)
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_CXX_STANDARD 17)

# CUDA settings
find_package(CUDA)
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_50") # Set GPU architecture (adjust if necessary)

# Include csv-parser as a submodule
set(CSV_PARSER_DIR "${CMAKE_SOURCE_DIR}/include/csv-parser")
include_directories(${CSV_PARSER_DIR}/single_include)

# Function to define an executable with common settings
function(add_kmeans_executable exe_name source_files cuda_files)

    # if cuda is found, add the executable with CUDA support
    if(CUDA_FOUND AND MSVC)
        add_definitions(-DUSE_CUDA)
        cuda_add_executable(${exe_name} ${source_files} ${cuda_files})
        target_link_libraries(${exe_name} ${CUDA_LIBRARIES} matplotlib_cpp ${Python3_LIBRARIES} ${OpenMP_CXX_LIBRARIES})
    else()
        add_executable(${exe_name} ${source_files})
        target_link_libraries(${exe_name} matplotlib_cpp ${Python3_LIBRARIES} ${OpenMP_CXX_LIBRARIES})
    endif()

    target_compile_options(${exe_name} PRIVATE ${OpenMP_CXX_FLAGS})
    target_include_directories(${exe_name} PRIVATE include ${Python3_INCLUDE_DIRS})
    set_target_properties(${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set_target_properties(${exe_name} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endfunction()

# Find Python3
set(Python3_ROOT_DIR "D:/Users/miche/anaconda3")  # Change to your Python installation
set(Python3_EXECUTABLE "D:/Users/miche/anaconda3/python.exe")
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
message(STATUS "Python3 Executable: ${Python3_EXECUTABLE}")
message(STATUS "Python3 Library: ${Python3_LIBRARIES}")
message(STATUS "Python3 Include Directory: ${Python3_INCLUDE_DIRS}")

# OpenMP settings
find_package(OpenMP REQUIRED)
set(ENV{OMP_NESTED} TRUE)
set(ENV{OMP_NUM_THREADS} 8)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp:llvm /openmp:experimental")
endif()

# Define executables
add_kmeans_executable(k_means "src/main.cpp" "include/k_means/kmeans.cu")
add_kmeans_executable(mesh_segmentation "src/segmentation.cpp;src/Mesh.cpp" "include/k_means/kmeans.cu")
add_kmeans_executable(evaluation "src/evaluation.cpp;src/Mesh.cpp" "include/k_means/kmeans.cu")
