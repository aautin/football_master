FROM ubuntu:22.04

# Install dependencies for Qt6 build + ninja
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-tools-dev \
    ninja-build \
    libgl1-mesa-dev \
    libopengl-dev \
    libxkbcommon-dev \
    libvulkan-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace