cmake_minimum_required(VERSION 3.10)
project(MainProject C)

# Establecer el estándar C
set(CMAKE_C_STANDARD 11)

# Ubicación de salida del ejecutable: carpeta raíz del proyecto
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Lista de archivos fuente
add_executable(main
    main.c
    Drivers/bacn_RF.c
    Modules/IQ.c
    Modules/CS8toIQ.c
    Modules/parameter.c
    Modules/welch.c
    Modules/cJSON.c
    Modules/moda.c
    Modules/find_closest_index.c
    Modules/handle_web.c
)

# Opciones de compilación
target_compile_options(main PRIVATE -g -fdiagnostics-color=always)

# Enlazar librerías necesarias
target_link_libraries(main fftw3 m hackrf pthread)