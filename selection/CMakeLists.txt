# cmake_minimum_required(VERSION <specify CMake version here>)
project(selection)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)
add_executable(selection main.cpp Arbitrage.cpp Arbitrage.h FilesManager.cpp FilesManager.h CurrencyPair.cpp CurrencyPair.h Triplet.cpp Triplet.h OutputFormat.cpp OutputFormat.h)