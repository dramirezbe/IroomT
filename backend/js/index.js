// server.js
require('dotenv').config();
const express = require('express');
const path = require('path');
const { createServer } = require('http');
const { Server } = require('socket.io');
const readJSONCallback = require('./handleJSON');

const app = express();
const PORT = process.env.PORT || 3001;
const httpServer = createServer(app);

const io = new Server(httpServer, {
  cors: {
    origin: process.env.FRONTEND_URL || "http://localhost:5173",
    methods: ["GET", "POST"]
  }
});

if (process.env.NODE_ENV === 'production') {
  app.use(express.static(path.join(__dirname, '../../frontend/dist')));
  app.get('*', (req, res) => {
    res.sendFile(path.resolve(__dirname, '../../frontend/dist', 'index.html'));
  });
} else {
  app.get('/', (req, res) => {
    res.send('[express] Working in development mode');
  });
}

httpServer.listen(PORT, '0.0.0.0', () => {
  console.log(`[express] Express server listening on port ${PORT}`);
});

function emitJSONData() {
  readJSONCallback((err, socketData) => {
    if (err) {
      console.error("[express] Error al leer el JSON:", err);
    } else {
      io.emit('jsonData', socketData);
      console.log('[express] JSON data emitted via socket');
    }
  });
}

setInterval(emitJSONData, 1000);
