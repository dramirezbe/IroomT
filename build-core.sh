#!/bin/bash
# init-core.sh
# Este script compila el proyecto C y ejecuta el binario resultante.
# Se asume que:
# - La carpeta "backend" está en la raíz del proyecto.
# - El directorio de compilación es "backend/build".
# - El binario se llama "monraf-core".

# Entrar al directorio backend
echo "-------- init build-core.sh --------"
cd backend/Core || { echo "No se pudo acceder a 'backend'"; exit 1; }

# Crear la carpeta build (si no existe) y entrar en ella
mkdir -p build && cd build || { echo "No se pudo acceder a 'build'"; exit 1; }

# Ejecutar cmake y make para compilar el proyecto C
echo "Ejecutando cmake .."
cmake ..
echo "Ejecutando make"
make

# Verificar que el binario existe
BINARY="./monraf-core"
if [ ! -f "$BINARY" ]; then
    echo "Error: no se encontró el binario $BINARY. Revisa la compilación."
    exit 1
fi

echo "-------- leaving build-core.sh --------"