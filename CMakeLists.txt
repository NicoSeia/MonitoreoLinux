cmake_minimum_required(VERSION 3.10)
project(monitoring_project)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(monitoring_project
        src/main.c
        src/metrics.c
        src/expose_metrics.c
)

link_directories(${CMAKE_SOURCE_DIR}/lib)

target_link_libraries(monitoring_project
        ${CMAKE_SOURCE_DIR}/lib/libprom.so
        ${CMAKE_SOURCE_DIR}/lib/libpromhttp.so
)
