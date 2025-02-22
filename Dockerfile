# Use Ubuntu as the base image
FROM ubuntu:latest

# Set non-interactive mode to prevent prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    git \
    ninja-build \
    libomp-dev \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /workspace

# Copy everything except `build/` (excluded by `.dockerignore`)
COPY . /workspace

# Ensure a clean build directory
RUN mkdir -p /workspace/build

# Configure and compile the project
WORKDIR /workspace/build
RUN cmake .. && make -j$(nproc)

# Run the compiled executable on container startup
CMD ["/bin/bash"]
