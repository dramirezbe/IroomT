// socketHandler.js
const TcpClient = require('./tcpClient');

function sendSocketData(socketData, callback) {
  // Si no se pasa callback, usamos una función vacía para evitar errores
  if (typeof callback !== 'function') {
    callback = () => {};
  }

  let message;
  try {
    // Suponemos que socketData es un array (o dato serializable) y lo convertimos directamente a JSON
    message = JSON.stringify(socketData);
  } catch (err) {
    console.error('[socketHandler] Error al convertir socketData a JSON:', err);
    return callback(err, null);
  }

  // Crea una instancia del cliente TCP
  const tcpClientInstance = new TcpClient();

  // Conecta al servidor TCP
  tcpClientInstance.connect();

  // Una vez conectado, se dispara el evento 'connect'
  tcpClientInstance.tcpClient.on('connect', () => {
    console.log('[socketHandler] Conectado, enviando socketData...');
    tcpClientInstance.tcpClient.write(message);
  });

  // Escucha la respuesta del servidor
  tcpClientInstance.tcpClient.on('data', (data) => {
    console.log('[socketHandler] Respuesta del servidor:', data.toString());
    callback(null, data.toString());
    // Cierra la conexión después de recibir la respuesta
    tcpClientInstance.tcpClient.destroy();
  });

  // Manejo de errores
  tcpClientInstance.tcpClient.on('error', (err) => {
    console.error('[socketHandler] Error:', err.message);
    callback(err, null);
  });
}

module.exports = sendSocketData;
