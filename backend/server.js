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

// Constantes
const DATA_POINTS = 4096;
const POINT_SIZE = 16; // 8 bytes (x) + 8 bytes (y)
const EXPECTED_BYTES = DATA_POINTS * POINT_SIZE;

// Variables de estado
let isStreaming = false;
let tcpClient = null;
let receivedData = Buffer.alloc(0);

// Función para manejar datos del servidor C
const handleDataFromC = (socket) => {
  tcpClient = new net.Socket();
  receivedData = Buffer.alloc(0);

  tcpClient.connect(C_SERVER_PORT, C_SERVER_HOST, () => {
    console.log('[TCP] C ✅');
  });

  tcpClient.on('data', (chunk) => {
    receivedData = Buffer.concat([receivedData, chunk]);

    // Verificar si hemos recibido todos los datos
    if (receivedData.length >= EXPECTED_BYTES) {
      try {
        const points = [];
        
        // Procesar exactamente los bytes esperados
        const validData = receivedData.subarray(0, EXPECTED_BYTES);
        
        for (let i = 0; i < DATA_POINTS; i++) {
          const offset = i * POINT_SIZE;
          const x = validData.readDoubleLE(offset);
          const y = validData.readDoubleLE(offset + 8);
          points.push({ x, y });
        }

        // Enviar datos a React
        socket.emit('data', points);
        
        // Cerrar conexión TCP
        tcpClient.end();
        receivedData = Buffer.alloc(0);

      } catch (err) {
        console.error('🚫 Processing data:', err.message);
      }
    }
  });

  tcpClient.on('close', () => {
    console.log('[TCP] C 🔽');
    tcpClient = null;
  });

  tcpClient.on('error', (err) => {
    console.error('[TCP] 🚫:', err.message);
    tcpClient = null;
  });
};

// Socket.IO - Manejo de clientes
io.on('connection', (socket) => {
  console.log('[WS] React client ✅');

  socket.on('start', () => {
    if (!isStreaming) {
      isStreaming = true;
      console.log('[WS] 🚗 streaming...');
      handleDataFromC(socket); // Primera solicitud
      
      // Configurar intervalo para solicitudes posteriores
      const interval = setInterval(() => {
        if (isStreaming && !tcpClient) {
          handleDataFromC(socket);
        }
      }, 1000); // Intervalo de 100ms

      socket.on('disconnect', () => {
        clearInterval(interval);
        isStreaming = false;
      });
    }
  });

  socket.on('stop', () => {
    if (isStreaming) {
      isStreaming = false;
      console.log('[WS] ✋ Streaming');
      if (tcpClient) tcpClient.end();
    }
  });
});

// Iniciar servidor
httpServer.listen(3001, () => {
  console.log('[WS] Server Node.js in http://localhost:3001');
});