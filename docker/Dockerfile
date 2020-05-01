FROM ubuntu:focal
LABEL maintainer="Antal Buss <docker@objektum.net>"

# Pre-Req Repos
RUN apt-get update \
 && apt-get install -y software-properties-common \

# Install pre-reqs
 && apt-get update \
 && apt-get install -y \
  build-essential \

# Install Tool
  g++-10 \
  clang \
  libc++-dev \
  libc++abi-dev \
  cmake
