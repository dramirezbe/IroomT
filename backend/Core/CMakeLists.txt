cmake_minimum_required(VERSION 3.10)
project(ServidorSignal C)

set(CMAKE_C_STANDARD 99)

# Buscar todos los archivos .c dentro de la carpeta Modules
file(GLOB MODULE_SOURCES "Modules/*.c")

# Crear el ejecutable incluyendo main.c y los módulos encontrados
add_executable(monraf-core main.c ${MODULE_SOURCES})

# Agregar la carpeta Modules a las rutas de inclusión
target_include_directories(monraf-core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Modules)

# Enlazar la librería matemática y pthread (esta última incluye funciones relacionadas con señales)
target_link_libraries(monraf-core m pthread)
