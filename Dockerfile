# Use a lightweight base image with C++ and Python preinstalled
FROM ubuntu:22.04

# Set environment variables to prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required system packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    python3 \
    python3-dev \
    python3-pip \
    git \
    && apt-get clean

# Install Python libraries for matplotlibcpp
RUN python3 -m pip install numpy matplotlib

# Set the working directory in the container
WORKDIR /app

# Copy your project files into the container
COPY . /app

# Build the project
RUN [ -d build ] && rm -rf build || true && \
    mkdir build && cd build && \
    cmake .. && \
    cmake --build . --config Release

# Set the default command to execute the compiled binary
CMD ["./build/bin/k_means"]
