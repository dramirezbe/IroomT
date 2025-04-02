const fs = require('fs');
const path = require('path');

// Construir la ruta relativa al archivo .js actual
const filePath = path.join(__dirname, '..', 'Core', 'JSON', '0');

function readJSONCallback(callback) {
  fs.readFile(filePath, 'utf8', (err, jsonString) => {
    if (err) {
      // Si ocurre un error al leer, reintentar después de 0.5 segundos
      return setTimeout(() => readJSONCallback(callback), 500);
    }
    try {
      const parsed = JSON.parse(jsonString);
      return callback(null, parsed);
    } catch (parseError) {
      // Si ocurre un error al parsear, reintentar después de 0.5 segundos
      return setTimeout(() => readJSONCallback(callback), 500);
    }
  });
}

module.exports = readJSONCallback;
