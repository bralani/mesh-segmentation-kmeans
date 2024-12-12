# K-Means Clustering Hands-On

Welcome to the **K-Means Clustering Hands-On** project! This repository provides an interactive and educational implementation of the **K-means clustering algorithm**. It's designed to help you understand how the algorithm works and experiment with clustering data.

---

## Features

- **Hands-On K-Means Implementation**: Learn how the K-means clustering algorithm works step-by-step.
- **Modular Design**: Easily configurable parameters for customization.
- **Simple Setup**: Get started with minimal configuration.
- **Visualizations**: View clustering results and centroids.

---

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
docker build -t kmeans-plot . # To build the image
docker run --rm -it -v $(pwd)/output:/app/output kmeans-plot # to run the image and store the plot
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