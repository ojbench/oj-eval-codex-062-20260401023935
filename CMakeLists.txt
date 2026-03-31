cmake_minimum_required(VERSION 3.10)
project(memoryriver_easyaoj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(code src/main.cpp)
# Header-only template; ensure include path contains src
 target_include_directories(code PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Optimize a bit for runtime if any
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

