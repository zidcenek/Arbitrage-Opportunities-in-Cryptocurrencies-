# cmake_minimum_required(VERSION <specify CMake version here>)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_executable(untitled main.cpp Arbitrage.cpp Arbitrage.h FilesManager.cpp FilesManager.h CurrencyPair.cpp CurrencyPair.h Triplet.cpp Triplet.h)