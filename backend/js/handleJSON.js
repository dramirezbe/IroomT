const fs = require('fs');
const path = require('path');

// Construir la ruta relativa al archivo .js actual
const filePath = path.join(__dirname, 'json', '0');

function readJSONCallback(callback) {
  fs.readFile(filePath, 'utf8', (err, jsonString) => {
    if (err) {
      console.error("Error al leer el archivo:", err);
      return callback(err, null);
    }
    try {
      const parsed = JSON.parse(jsonString);
      return callback(null, parsed);
    } catch (parseError) {
      console.error("Error al parsear el JSON:", parseError);
      return callback(parseError, null);
    }
  });
}

module.exports = readJSONCallback;
