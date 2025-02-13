require('dotenv').config();
const express = require('express');
const aedes = require('aedes')();
const { createServer } = require('http');
const ws = require('websocket-stream');
const mqtt = require('mqtt');

// Configuración
const HTTP_PORT = process.env.HTTP_PORT || 5883;
const WS_PORT = process.env.WS_PORT || 4883;

// Servidor Express para endpoints adicionales
const app = express();
app.use(require('cors')({
  origin: 'http://localhost' // o tu dominio
}));

// Servidor MQTT sobre WebSocket
const httpServer = createServer();
ws.createServer({ server: httpServer }, aedes.handle);

// Lógica del contador
let counter = 0;
const client = mqtt.connect(`ws://localhost:${WS_PORT}`);

client.on('connect', () => {
  client.subscribe('counter/request');
});

client.on('error', (err) => {
  console.error('MQTT Error:', err);
});

client.on('message', (topic) => {
  if (topic === 'counter/request') {
    counter++;
    client.publish('counter/response', counter.toString());
  }
});

// Iniciar servidores
httpServer.listen(WS_PORT, () => {
  console.log(`MQTT WebSocket en puerto ${WS_PORT}`);
});

app.listen(HTTP_PORT, () => {
  console.log(`API HTTP en puerto ${HTTP_PORT}`);
});