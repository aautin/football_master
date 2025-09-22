FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

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
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

CMD ["tail", "-f", "/dev/null"]