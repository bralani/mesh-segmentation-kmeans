<img src="docs/img/logo.jpg" alt="logo" width="600px"/>

# Mesh Segmentation with K-Means
 This repository provides an interactive implementation of **mesh segmentation**, leveraging the **K-Means clustering algorithm**. You can use this project to segment 3D meshes efficiently while still having the flexibility to apply K-Means clustering independently.

## Features
- Flexible K-Means Usage: The K-Means implementation can also be used separately for general clustering tasks, offering versatility.
- Mesh Segmentation Using Dijkstra's Algorithm: Utilize Dijkstra's algorithm for an alternative segmentation method, focusing on shortest paths within the mesh.
- Mesh Segmentation Using Heat Equation: Segment 3D models based on the heat equation, providing a smooth and efficient way to divide the mesh into distinct regions.
- Centroid Initialization Methods: Support for various initialization techniques, including random, most distant points, and density-based approaches to improve clustering results.
- Automatic K-Detection: Automatically determine the optimal number of clusters using methods like silhouette scores and the elbow method.
- Mesh Exporting: Export segmented meshes for further analysis or processing in different formats.
- Visualization Tools: View segmented meshes in an interactive window with color-coded clusters, making it easier to interpret the results visually.
- Parallel Processing: Utilize OpenMP to improve performance by parallelizing tasks.
- GPU Implementation for K-Means: Accelerate the K-Means algorithm with GPU implementations for each of its steps, ensuring faster computations on large datasets.

## Requirements

To run the project, ensure you have the following dependencies installed:

On macOS you can install the dependencies using [Homebrew](https://brew.sh/). On Linux, you can use `apt-get` to install the dependencies. On Windows, you can download the dependencies from their official websites.

1) C++ Compiler
    - macOS: Clang (we have used Clang 15.0.0)
    - Linux: GCC (we have used GCC 14.2.0)
    - Windows: MSVC 2019 or later
2) CMake
    - macOS:
      ```bash
      brew install cmake
      ```
    - Linux:
      ```bash
      sudo apt-get install cmake
      ```
    - Windows:
      Download and install from the [official website](https://cmake.org/download/). Ensure it's added to the system PATH.
3) OpenMP
    - macOS:
      ```bash
      brew install libomp
      ```
    - Linux:
      ```bash
      sudo apt-get install libomp-dev
      ```
    - Windows:
      OpenMP is included in MSVC by default.

4) Viewer dependencies (optional but strongly recommended):
- OpenGL, GLM, GLFW
    - macOS:
      ```bash
      brew install glfw glm
      ```
    - Linux:
      ```bash
      sudo apt update
      sudo apt install libgl1-mesa-dev libglu1-mesa-dev
      sudo apt install libglfw3-dev
      sudo apt install libglm-dev
      ```
    - Windows:
      ```bash
      git clone https://github.com/microsoft/vcpkg.git
      cd vcpkg
      ./bootstrap-vcpkg.bat
      ```
      From now on, consider C:/path/to/vcpkg as your right path to vcpkg, and replace in each occurrence.
      Then, run these commands in terminal:
      ```bash
      $env:VCPKG_ROOT = "C:/path/to/vcpkg"
      $env:PATH = "$env:VCPKG_ROOT;$env:PATH"
      vcpkg install opengl glfw3 glm
      ```
      Then, go to `./src/model_renderer/CMakeLists.txt` and replace the two variables `GLFW_INCLUDE_DIR` and `GLFW_LIBRARY` with:
      - `GLFW_INCLUDE_DIR: C:/path/to/vcpkg/packages/glfw3_x64-windows/include`
      - `GLFW_LIBRARY: C:/path/to/vcpkg/packages/glfw3_x64-windows/lib/glfw3dll.lib`

5) GPU CUDA only for Windows MSVC (Optional):
    - Ensure you have MSVC 2019 or later installed.
    - Download and install CUDA Toolkit from the [official website](https://developer.nvidia.com/cuda-downloads).
    - Verify installation:
      ```bash
      nvcc --version
      ```

## Setup and Installation

Follow these steps to build and run the project:

### 1. Clone the Repository
```bash
git clone https://github.com/AMSC-24-25/16-kmeans-16-kmeans.git
cd 16-kmeans-16-kmeans
```

### 2. Download and install dependencies
```bash
git submodule update --init --recursive
```

### 3. Build the Project
```bash
mkdir build
cd build
cmake ..
make
```

## Dataset

To run the project, download the required 3D dataset from the following [link](https://polimi365-my.sharepoint.com/:u:/g/personal/10978268_polimi_it/EZKJJOmNr_REh4EHY5Tln7QBmNEsD940wz2wfekhq0LguA?e=noOjtN). Unzip the files and place them in the `resources` folder. The folder structure should look like this:
```bash
root/
├── build/
├── docs/
├── include/   
├── output/           
├── resources/        
│   ├── meshes/
│       ├── obj/
│       │   ├── *.obj
│       ├── seg/
│           ├── *.seg
├── src/     
├── tests/
├── third_party/
```
The dataset contains 400 3D models in OBJ format, along with their corresponding segmentation files. The segmentation files are in the `.seg` format, where each line corresponds to a face and its cluster index. There are multiple `.seg` files for each 3D model, each representing a different segmentation from a different human annotator. The dataset has been obtained from the [Princeton Segmentation Benchmark](http://segeval.cs.princeton.edu/).

## Running with Docker
```bash
# From the main folder
docker build -t mesh-segmentation .  # Build the Docker image
docker run --rm -it -v $(pwd)/output:/app/output mesh-segmentation  # Run the container
```

## Getting started

Once the project is built, you can run the segmentation tool with:
```bash
./build/segmentation <path-to-mesh-file> <number-of-clusters>
```
Example:
```bash
k-means/
├── data/             # Folder for datasets (optional)
├── build/            # Build artifacts (created after compilation)
├── src/              # Source code for the project
├── Makefile    # Build configuration for CMake
├── README.md         # Project documentation
└── kmeans.cpp        # Main implementation of K-means algorithm
```

### 5. How to run tests

```bash
brew install lcov #MacOS
sudo apt install lcov #Ubuntu
brew install ninja

# Verify installation
lcov --version
genhtml --version

```
