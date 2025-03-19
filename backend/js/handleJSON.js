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
      // Extraer los datos que necesitas del objeto "data"
      const { band, fmin, fmax, units, measure, vectors } = parsed.data;
      const { Pxx, f } = vectors;
      const socketData = [band, fmin, fmax, units, measure, Pxx, f];
      
      // Llamamos al callback con null como error y socketData como datos
      callback(null, socketData);
    } catch (error) {
      console.error("Error parseando JSON:", error);
      callback(error, null);
    }
  });
}


module.exports = readJSONCallback;