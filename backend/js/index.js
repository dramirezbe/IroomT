// index.js
const path = require('path');
require('dotenv').config({
  path: path.resolve(__dirname, '../../.env')
});

const express = require('express');
const { createServer } = require('http');
const { Server } = require('socket.io');

const jsonDir = process.env.CORE_JSON_PATH;
if (!jsonDir) {
  console.error('[index] ERROR: CORE_JSON_PATH not set in .env');
  process.exit(1);
}

const createJSONReader = require('./handleJSON');
const readJSON = createJSONReader(jsonDir);

console.log('Using JSON directory:', jsonDir);

const app = express();
const PORT = process.env.VITE_BUILD_PORT || 3001;
const serverIP = process.env.VITE_SERVER_IP;
const httpServer = createServer(app);
const io = new Server(httpServer, {
  cors: {
    origin: "*",
    methods: ['GET', 'POST']
  }
});

if (process.env.NODE_ENV === 'production') {
  const webPath = process.env.WEB_BUILD_PATH;
  app.use(express.static(webPath));
  app.get('*', (req, res) => {
    res.sendFile(path.resolve(webPath, 'index.html'));
  });
} else {
  app.get('/', (req, res) => res.send('[express] Development mode'));
}

httpServer.listen(PORT, '0.0.0.0', () => {
  console.log(`[express] Listening at http://${serverIP}:${PORT}`);
});

function emitJSONData() {
  readJSON((err, data) => {
    if (err) {
      console.error('[index] JSON read+parse failed 10x, exiting:', err.message);
      process.exit(1);
    }
    io.emit('jsonData', data);
    console.log('[index] Emitted JSON for index');
  });
}

// send (or fail+exit) every second
setInterval(emitJSONData, 1000);
