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
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /workspace

# Default command (can be overridden)
CMD ["/bin/bash"]
