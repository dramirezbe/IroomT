#!/bin/bash
# init-core.sh
# Asegura rutas correctas independientemente de dónde se ejecute el script

# Obtener directorio del script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "-------- init build-core.sh --------"
cd "$SCRIPT_DIR/backend/Core" || { echo "No se pudo acceder a 'backend'"; exit 1; }

mkdir -p build && cd build || { echo "Error con 'build'"; exit 1; }

echo "Executing cmake .."
cmake ..
echo "Executing make"
make
echo "-------- leaving build-core.sh --------"