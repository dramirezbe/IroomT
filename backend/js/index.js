const express = require('express');
const path = require('path');
const { createServer } = require('http');
const { Server } = require('socket.io');
const handleCore = require('./handleCore'); // Asegúrate de que la ruta sea correcta

const app = express();
const PORT = process.env.PORT || 3001;
const httpServer = createServer(app);
const io = new Server(httpServer, {
  cors: {
    origin: "http://localhost:5173",
    methods: ["GET", "POST"]
  }
});

// Configuración para servir el frontend en producción
if (process.env.NODE_ENV === 'production') {
  app.use(express.static(path.join(__dirname, '../../frontend/dist')));
  // Manejar rutas con React Router
  app.get('*', (req, res) => {
    res.sendFile(path.resolve(__dirname, '../../frontend/dist', 'index.html'));
  });
} else {
  // Ruta de prueba en desarrollo
  app.get('/', (req, res) => {
    res.send('API funcionando en desarrollo');
  });
}

// Inicializa el manejo de Socket.io y la conexión TCP
require('./socketHandler')(io);

httpServer.listen(PORT, () => {
  console.log(`Servidor corriendo en http://localhost:${PORT}`);
  handleCore.initCore()
    .then(() => {
      handleCore.startCore();
    })
    .catch((err) => {
      console.error('Error durante la inicialización del core:', err);
    });
});
