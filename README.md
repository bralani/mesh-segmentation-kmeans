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