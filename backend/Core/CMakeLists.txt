cmake_minimum_required(VERSION 3.10)
project(ServidorSignal C)

set(CMAKE_C_STANDARD 99)

# Agregar los archivos fuente
add_executable(monraf-core main.c handle_data.c)

# Enlazar la librería matemática
target_link_libraries(monraf-core m)
