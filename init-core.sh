#!/bin/bash
# init-core.sh
# Asegura rutas correctas independientemente de dónde se ejecute el script

# Obtener directorio del script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"




cd "$SCRIPT_DIR/backend/Core" || { echo "No se pudo acceder a 'backend'"; exit 1; }

echo "-------- init core--------"
chmod +x main
./main