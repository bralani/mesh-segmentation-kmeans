cmake_minimum_required(VERSION 3.10)
project(KMeans)

# Set the target architecture for macOS (arm64)
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_CXX_STANDARD 17)

# Coverage only if CLANG or GCC
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

add_definitions(-DROOT_FOLDER=\"${CMAKE_SOURCE_DIR}\")

# CUDA settings
find_package(CUDA)
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_50") # Set GPU architecture (adjust if necessary)

# Include dependencies
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/csv-parser/single_include)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/eigen)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/libigl/include)
add_subdirectory(src/model_renderer)

# TESTING
add_subdirectory(tests)

# BENCHMARKING 
set(BENCHMARK_ENABLE_TESTING OFF)  
add_subdirectory(third_party/benchmark)

# Function to define an executable with common settings
function(add_kmeans_executable exe_name source_files cuda_files)
    if(CUDA_FOUND AND MSVC)
        add_definitions(-DUSE_CUDA)
        cuda_add_executable(${exe_name} ${source_files} ${cuda_files})
        target_link_libraries(${exe_name} ${CUDA_LIBRARIES})
        set_target_properties(${exe_name} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
    else()
        add_executable(${exe_name} ${source_files})
    endif()

    target_link_libraries(${exe_name} OpenMP::OpenMP_CXX)
    if(TARGET model_renderer)
        target_link_libraries(${exe_name} model_renderer)
    endif()

    target_compile_options(${exe_name} PRIVATE ${OpenMP_CXX_FLAGS})
    target_include_directories(${exe_name} PRIVATE include third_party)
    set_target_properties(${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endfunction()

# OpenMP settings
find_package(OpenMP REQUIRED)
set(ENV{OMP_NESTED} TRUE)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp:llvm /openmp:experimental")
endif()

file(GLOB_RECURSE SOURCES "src/geometry/*.cpp" "src/clustering/*.cpp")
file(GLOB_RECURSE CUDA_SOURCES "include/*.cu")

# Define executables
add_kmeans_executable(k_means "src/main.cpp;${SOURCES}" "${CUDA_SOURCES}")
add_kmeans_executable(mesh_segmentation "src/segmentation.cpp;${SOURCES}" "${CUDA_SOURCES}")
add_kmeans_executable(evaluation "src/evaluation.cpp;${SOURCES}" "${CUDA_SOURCES}")

if(TARGET model_renderer)
    add_kmeans_executable(viewer "src/viewer.cpp;${SOURCES}" "${CUDA_SOURCES}")
    if(WIN32)
            add_custom_command(TARGET viewer POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${GLFW_INCLUDE_DIR}/../bin/glfw3.dll"
                    $<TARGET_FILE_DIR:viewer>)
    endif()
endif()

add_kmeans_executable(kmeans_benchmark "src/benchmark.cpp;${SOURCES}" ${CUDA_SOURCES})
target_link_libraries(kmeans_benchmark benchmark::benchmark)