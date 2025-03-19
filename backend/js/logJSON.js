// log_JSON.js
const readJSONCallback = require('./handleJSON'); // Asegúrate de ajustar la ruta al módulo

readJSONCallback((err, data) => {
  if (err) {
    return console.error("Error al leer el JSON:", err);
  }
  //console.log("Datos leídos:", data);
  const [
    band = "N/A",
    fmin = "N/A",
    fmax = "N/A",
    units = "N/A",
    measure = "N/A",
    Pxx = [],
    f = []
  ] = data;
  console.log("Tipo de datos:", typeof band);
  console.log("Valor de fmin:", typeof fmin);
  console.log("Valor de fmax:", typeof fmax);
  console.log("Valor de units:", typeof units);
  console.log("Valor de measure:", typeof measure);
  console.log("Valor de Pxx:", typeof Pxx);
  console.log("Valor de f:", typeof f);

  console.log("Tipo de datos:", band);
  console.log("Valor de fmin:", fmin);
  console.log("Valor de fmax:", fmax);
  console.log("Valor de units:", units);
  console.log("Valor de measure:", measure);
  console.log("Valor de Pxx:", Pxx);
  console.log("Valor de f:", f);
});
