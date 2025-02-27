const TcpClient = require('./tcpClient');

module.exports = (io) => {
  const tcpClient = new TcpClient();
  let streamingInterval = null;
  let isStreaming = false;

  // Define el callback para emitir datos a todos los clientes conectados
  tcpClient.onData = (points) => {
    io.emit('data', points);
  };

  io.on('connection', (socket) => {
    console.log('[WS] Cliente conectado');

    socket.on('start', (frequencies) => {
      if (frequencies && frequencies.length === 2) {
        console.log('[WS] Iniciando streaming con frecuencias:', frequencies);
        if (!isStreaming) {
          isStreaming = true;
          if (!tcpClient.isConnected) {
            tcpClient.connect(frequencies);
          } else {
            tcpClient.updateFrequencies(frequencies);
          }
          // Envía la solicitud de datos cada 1 segundo
          streamingInterval = setInterval(() => {
            tcpClient.requestData();
          }, 1000);
        }
      }
    });

    socket.on('stop', () => {
      if (isStreaming) {
        isStreaming = false;
        console.log('[WS] Streaming detenido');
        if (streamingInterval) {
          clearInterval(streamingInterval);
          streamingInterval = null;
        }
        tcpClient.stop();
      }
    });

    socket.on('disconnect', () => {
      console.log('[WS] Cliente desconectado');
      if (streamingInterval) {
        clearInterval(streamingInterval);
        streamingInterval = null;
      }
      isStreaming = false;
    });
  });
};
