cmake_minimum_required(VERSION 3.20)
project(TakumiCup CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# --- geom: cone intersection, h0 lookup, wall/LED mapping ---
add_library(geom STATIC
    geom/ConeGeometry.cpp
    geom/TiltedSurface.cpp
    geom/LedMapping.cpp
)
target_include_directories(geom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- sim: g-force-driven tilt oscillator (no fluid PDE) + LED on/off state.
# Pure math, no platform deps, must stay ESP32-clean; depends only on geom
# (also portable). This is the code that will actually run on the device.
add_library(sim STATIC
    sim/TiltOscillator.cpp
    sim/LedState.cpp
)
target_include_directories(sim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sim PUBLIC geom)

# --- input: UDP receiver (future) + replay driver ---
add_library(input STATIC
    input/ReplayDriver.cpp
)
target_include_directories(input PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(input PUBLIC sim)

# --- headless replay runner (no viz yet) ---
add_executable(replay_main apps/replay_main.cpp)
target_link_libraries(replay_main PRIVATE sim geom input)

enable_testing()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
    add_subdirectory(tests)
endif()
