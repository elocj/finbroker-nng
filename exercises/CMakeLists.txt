# Find NNG library (reuse from parent)
find_library(NNG_LIBRARY nng)
find_path(NNG_INCLUDE_DIR nng/nng.h)

set(NNG_LIBRARIES ${NNG_LIBRARY})
set(NNG_INCLUDE_DIRS ${NNG_INCLUDE_DIR})
set(NNG_FOUND TRUE)

# REQ/REP example
add_executable(reqrep reqprep.cpp)
target_link_libraries(reqrep ${NNG_LIBRARIES})
target_include_directories(reqrep PRIVATE ${NNG_INCLUDE_DIRS})

# Set output directory
set_target_properties(reqrep PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/exercises
)