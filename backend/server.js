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
let tcpClient = new net.Socket();
let isConnected = false;
let receivedData = Buffer.alloc(0);
let streamingInterval = null;

// Función para conectar de forma persistente a C
const connectToC = (frequencies) => {
  tcpClient.connect(C_SERVER_PORT, C_SERVER_HOST, () => {
    console.log('[TCP] Conectado a C de forma persistente');
    // Envía las frecuencias iniciales (solo una vez al conectar)
    const freqBuffer = Buffer.alloc(16);
    freqBuffer.writeDoubleLE(frequencies[0], 0);
    freqBuffer.writeDoubleLE(frequencies[1], 8);
    tcpClient.write(freqBuffer);
    isConnected = true;
  });
};

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
      // Emite los datos a todos los clientes conectados vía Socket.io
      io.emit('data', points);
      // Elimina de receivedData la porción ya procesada
      receivedData = receivedData.subarray(EXPECTED_BYTES);
    } catch (err) {
      console.error('[TCP] Error procesando datos:', err.message);
    }
  }
});

tcpClient.on('error', (err) => {
  console.error('[TCP] Error:', err.message);
  isConnected = false;
});

tcpClient.on('close', () => {
  console.log('[TCP] Conexión cerrada');
  isConnected = false;
});

io.on('connection', (socket) => {
  console.log('[WS] Cliente conectado');

  socket.on('start', (frequencies) => {
    if (frequencies && frequencies.length === 2) {
      console.log('[WS] Iniciando streaming con frecuencias:', frequencies);
      if (!isStreaming) {
        isStreaming = true;
        if (!isConnected) {
          // Si aún no está conectado, se conecta y se envían las frecuencias iniciales
          connectToC(frequencies);
        } else {
          // Si ya está conectado, se envía un comando de actualización de frecuencias
          const updateBuffer = Buffer.alloc(20); // 4 bytes para el comando + 16 bytes para las frecuencias
          updateBuffer.write("FREQ", 0, 4, 'utf8');
          updateBuffer.writeDoubleLE(frequencies[0], 4);
          updateBuffer.writeDoubleLE(frequencies[1], 12);
          tcpClient.write(updateBuffer);
        }
        // Envía solicitud "REQ" cada 1 segundo para obtener nuevos datos de C
        streamingInterval = setInterval(() => {
          if (isConnected) {
            tcpClient.write(Buffer.from("REQ"));
          }
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
      if (isConnected) {
        // Enviar comando "STOP" para notificar a C que deje de enviar datos temporalmente
        tcpClient.write(Buffer.from("STOP"));
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
