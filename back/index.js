const { exec } = require('child_process');

// Ejecutar el comando
exec('./back/seno -t 2 -f 50 -s 1000', (error, stdout, stderr) => {
  if (error) {
    console.error(`Error al ejecutar el comando: ${error.message}`);
    return;
  }

  if (stderr) {
    console.error(`Error en la salida del comando: ${stderr}`);
    return;
  }

  try {
    // Parsear la salida JSON
    const data = JSON.parse(stdout);
    console.log('Datos recibidos:', data);
  } catch (err) {
    console.error(`Error al parsear el JSON: ${err instanceof Error ? err.message : 'Error desconocido'}`);
  }
});