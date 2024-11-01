cmake_minimum_required(VERSION 3.10)
project(monitoring_project)

# Establecer el tipo de construcción por defecto
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release) 
endif()

include(/build/Release/generators/conan_toolchain.cmake)

# Incluir directorios de encabezados
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CONAN_INCLUDE_DIRS}) 

# Añadir el ejecutable
add_executable(monitoring_project
    src/main.c
    src/metrics.c
    src/expose_metrics.c
    src/config.c
)

add_library(monitoring_project_lib STATIC
    src/metrics.c
    src/expose_metrics.c
    src/config.c
)

# Añadir el directorio donde se instalan las librerías compartidas (instaladas con sudo make install)
link_directories(/usr/local/lib)

target_link_libraries(monitoring_project PRIVATE monitoring_project_lib)


find_package(cJSON REQUIRED)

# Vincular las librerías libprom y libpromhttp desde /usr/local/lib
target_link_libraries(monitoring_project_lib
    /usr/local/lib/libprom.so
    /usr/local/lib/libpromhttp.so
    cjson::cjson
)

