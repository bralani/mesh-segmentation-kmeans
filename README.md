# K-Means Clustering Hands-On

Welcome to the **K-Means Clustering Hands-On** project! This repository provides an interactive and educational implementation of the **K-means clustering algorithm**. It's designed to help you understand how the algorithm works and experiment with clustering data.

## Introduction to K-Means Clustering

The **K-Means clustering algorithm** is a popular unsupervised machine learning method used to group data into clusters based on their features. It's widely used in various fields, including image processing, market segmentation, and bioinformatics.

### How It Works
1. **Initialization**: The algorithm starts by selecting `K` initial centroids, which can be chosen randomly or using specific strategies.
2. **Assignment Step**: Each data point is assigned to the nearest centroid, forming clusters.
3. **Update Step**: The centroids are recalculated as the mean of all data points assigned to them.
4. **Iteration**: Steps 2 and 3 are repeated until centroids stabilize (i.e., do not change significantly) or a maximum number of iterations is reached.

K-Means aims to minimize the **intra-cluster variance**, ensuring that data points within a cluster are as close as possible to their centroid. While simple and efficient, the algorithm may converge to a local minimum and can be sensitive to the initial placement of centroids.

## Features

- **Hands-On K-Means Implementation**: Learn how the K-means clustering algorithm works step-by-step.
- **Modular Design**: Easily configurable parameters for customization.
- **Simple Setup**: Get started with minimal configuration.
- **Visualizations**: View clustering results and centroids.

## Requirements
To run the project, you need the following tools and libraries:
- **C++ Compiler**: To compile and run the source code.
- **CMake**: To build the project.
- **Python with matplotlib**: To generate plots in 2D.
- **OpenMP**: For parallel processing.
- **Meshlib**: To load 3D models. Please install the release build following this guide: [https://meshlib.io/documentation/MeshLibCppSetupGuide.html](https://meshlib.io/documentation/MeshLibCppSetupGuide.html).
- **Docker**: To run the project in a container (optional).

## Data
In order to run the project, you must download the 3D dataset from the following [link](https://polimi365-my.sharepoint.com/:u:/g/personal/10978268_polimi_it/EVFMeQt8fdRCjSt8ZlpPfgcBnbx4x689MhNPFvU4Sx7S_A?e=obGYGv), unzip it and place it in the `resources` folder.

## Setup and Installation

Follow these steps to get the project up and running:

### 1. Clone the repository

```bash
git clone <repository-url>
cd <repository-folder>
```


### 2. Initialize and update submodules

```bash
git submodule update --init --recursive
```

### 3. Build the project
```bash
mkdir build
cd build
cmake ..
make
```

### 4. Docker commands
```bash
#From the main folder
docker build -t kmeans-plot . # To build the image, you may need to use "sudo"
docker run --rm -it -v $(pwd)/output:/app/output kmeans-plot # to run the image and store the plot, you may need to use "sudo"
```

### 4. Project Structure
```bash
k-means/
├── data/             # Folder for datasets (optional)
├── build/            # Build artifacts (created after compilation)
├── src/              # Source code for the project
├── Makefile    # Build configuration for CMake
├── README.md         # Project documentation
└── kmeans.cpp        # Main implementation of K-means algorithm
```