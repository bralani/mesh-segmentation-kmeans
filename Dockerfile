# Use the official GCC image as the base image
FROM gcc:latest

# Install required dependencies (Python, pip, NumPy, and Matplotlib for C++)
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-dev \
    python3-numpy \
    libpython3-dev \
    && pip3 install matplotlib numpy --break-system-packages

# Set the working directory inside the container
WORKDIR /usr/src/app

# Copy the C++ source code and the matplotlib-cpp headers into the container
COPY src/ ./src/
COPY include/matplotlib-cpp /usr/src/app/include/matplotlib-cpp/

# Compile the C++ program
RUN g++ -std=c++17 src/plot.cpp -o plot \
    `python3-config --cflags` `python3-config --ldflags` \
    -I/usr/src/app/include -L/usr/local/lib

# Set the default command to run the C++ program
CMD ["./plot"]
