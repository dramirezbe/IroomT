// server.js
const express = require('express');
const { createServer } = require('http');
const { Server } = require('socket.io');
const net = require('net');

const app = express();
const httpServer = createServer(app);
const io = new Server(httpServer, {
  cors: {
    origin: "http://localhost:5173",
    methods: ["GET", "POST"]
  }
});

// Configuración del servidor C
const C_SERVER_HOST = 'localhost';
const C_SERVER_PORT = 8080;

// Constantes para procesar los datos recibidos de C
const DATA_POINTS = 4096;
const POINT_SIZE = 16; // 8 bytes (x) + 8 bytes (y)
const EXPECTED_BYTES = DATA_POINTS * POINT_SIZE;

// Variables de estado globales
let isStreaming = false;
let tcpClient = null;
let receivedData = Buffer.alloc(0);

// Función para conectar y recibir datos desde el servidor C
const handleDataFromC = (socket, frequencies) => {
  tcpClient = new net.Socket();
  receivedData = Buffer.alloc(0);

  tcpClient.connect(C_SERVER_PORT, C_SERVER_HOST, () => {
    console.log('[TCP] C conectado');

    // Enviar las 2 frecuencias a C (16 bytes: 2 doubles, little-endian)
    const freqBuffer = Buffer.alloc(16);
    freqBuffer.writeDoubleLE(frequencies[0], 0);
    freqBuffer.writeDoubleLE(frequencies[1], 8);
    tcpClient.write(freqBuffer);
  });

  tcpClient.on('data', (chunk) => {
    receivedData = Buffer.concat([receivedData, chunk]);

    if (receivedData.length >= EXPECTED_BYTES) {
      try {
        const points = [];
        const validData = receivedData.subarray(0, EXPECTED_BYTES);
        for (let i = 0; i < DATA_POINTS; i++) {
          const offset = i * POINT_SIZE;
          const x = validData.readDoubleLE(offset);
          const y = validData.readDoubleLE(offset + 8);
          points.push({ x, y });
        }
        socket.emit('data', points);
        tcpClient.end();
        receivedData = Buffer.alloc(0);
      } catch (err) {
        console.error('🚫 Error procesando datos:', err.message);
      }
    }
  });

  tcpClient.on('close', () => {
    console.log('[TCP] C desconectado');
    tcpClient = null;
  });

  tcpClient.on('error', (err) => {
    console.error('[TCP] Error:', err.message);
    tcpClient = null;
  });
};

io.on('connection', (socket) => {
  console.log('[WS] Cliente React conectado');

  // Variable para almacenar el intervalo de streaming
  let streamingInterval = null;

  socket.on('start', (frequencies) => {
    if (!isStreaming && frequencies && frequencies.length === 2) {
      isStreaming = true;
      console.log('[WS] Iniciando streaming con frecuencias:', frequencies);
      handleDataFromC(socket, frequencies);

      // Crea un intervalo que se ejecuta cada 1 segundo
      streamingInterval = setInterval(() => {
        if (isStreaming && !tcpClient) {
          handleDataFromC(socket, frequencies);
        }
      }, 1000);
    }
  });

  socket.on('stop', () => {
    if (isStreaming) {
      isStreaming = false;
      console.log('[WS] Streaming detenido');
      if (tcpClient) tcpClient.end();
      if (streamingInterval) {
        clearInterval(streamingInterval);
        streamingInterval = null;
      }
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

httpServer.listen(3001, () => {
  console.log('[WS] Servidor Node.js corriendo en http://localhost:3001');
});
