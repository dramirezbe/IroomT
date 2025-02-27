// handleCore.js
const { spawn } = require('child_process');
const path = require('path');
const fs = require('fs');

let coreProcess;


function initCore() {
  return new Promise((resolve, reject) => {
    // Subir dos niveles y buscar el script build-core.sh en la raíz
    const scriptPath = path.join(__dirname, '..', '..', 'build-core.sh');

    // Ejecutar el script usando sh, con cwd apuntando a la raíz del proyecto
    const coreProcess = spawn('sh', [scriptPath], { 
      stdio: 'inherit', 
      cwd: path.join(__dirname, '..', '..')
    });

    coreProcess.on('error', (err) => {
      console.error('Error al ejecutar build-core.sh:', err);
      reject(err);
    });

    coreProcess.on('exit', (code, signal) => {
      console.log(`build-core.sh terminó con código ${code} y señal ${signal}`);
      resolve();
    });
  });
}

function startCore() {
  // Subir una carpeta, entrar a 'Core' y luego buscar build/monraf-core
  const binaryPath = path.join(__dirname, '..', 'Core', 'build', 'monraf-core');

  if (!fs.existsSync(binaryPath)) {
    console.error(`No se encontró el binario en ${binaryPath}. Verifica la compilación.`);
    return;
  }

  // Ejecutar el programa C
  const coreProcess = spawn(binaryPath, [], { stdio: 'inherit' });

  coreProcess.on('error', (err) => {
      console.error('Error al ejecutar el programa C:', err);
  });

  coreProcess.on('exit', (code, signal) => {
      console.log(`El programa C terminó con código ${code} y señal ${signal}`);
  });
}


/**
 * Detiene el proceso C.
 */
function stopCore() {
  if (coreProcess) {
    console.log('Deteniendo el programa C...');
    coreProcess.kill();
  }
}

module.exports = { initCore, startCore, stopCore };
