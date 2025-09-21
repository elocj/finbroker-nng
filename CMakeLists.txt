cmake_minimum_required(VERSION 3.16)
project(finbroker-nng VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(NNG_LIBRARY nng)
find_path(NNG_INCLUDE_DIR nng/nng.h)
if(NNG_LIBRARY AND NNG_INCLUDE_DIR)
    set(NNG_LIBRARIES ${NNG_LIBRARY})
    set(NNG_INCLUDE_DIRS ${NNG_INCLUDE_DIR})
    set(NNG_FOUND TRUE)
endif()

# Create executable
add_executable(finbroker-nng
    src/main.cpp
    src/finbroker.cpp
    src/finpublisher.cpp
    src/finsubscriber.cpp
    src/metrics/brokermetrics.cpp)

# Link NNG library
target_link_libraries(finbroker-nng ${NNG_LIBRARIES})
target_include_directories(finbroker-nng PRIVATE ${NNG_INCLUDE_DIRS})

# Set output directory
set_target_properties(finbroker-nng PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add exercises subdirectory
add_subdirectory(exercises)