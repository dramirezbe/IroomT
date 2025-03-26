const net = require('net');
const readJSONCallback = require('./handleJSON');

const C_SERVER_HOST = 'localhost';
const C_SERVER_PORT = 8080;
const RECONNECT_DELAY = 2000; // 2 segundos de espera antes de reintentar

class TcpClient {
  /**
   * @param {function} onJsonData - Función callback que se invoca al leer el JSON.
   */
  constructor(onJsonData) {
    this.tcpClient = new net.Socket();
    this.isConnected = false;
    this.receivedData = Buffer.alloc(0);
    this.onJsonData = onJsonData;
    this.setupListeners();
  }

  setupListeners() {
    this.tcpClient.on('data', (chunk) => {
      // Concatenar los datos recibidos
      this.receivedData = Buffer.concat([this.receivedData, chunk]);
      
      // Verificar si se ha recibido al menos 1 byte (el flag)
      if (this.receivedData.length >= 1) {
        const flag = this.receivedData.readUInt8(0);
        
        if (flag === 1) {
          console.log('[TCPs] JSON created');
          // Ejecutar el callback para leer el JSON
          readJSONCallback((err, socketData) => {
            if (err) {
              console.error("There was an error reading JSON:", err);
            } else {
              console.log("[TCPc] Read data from JSON");
              // Llamar al callback pasado en el constructor para enviar los datos vía socket.io
              if (this.onJsonData) {
                this.onJsonData(socketData);
              }
            }
          });
        }
        
        // Eliminar el byte ya procesado
        this.receivedData = this.receivedData.slice(1);
      }
    });

    this.tcpClient.on('error', (err) => {
      console.error('[TCPc] Error:', err.message);
      this.isConnected = false;
      this.retryConnection();
    });

    this.tcpClient.on('close', () => {
      console.log('[TCPc] Connection Closed');
      this.isConnected = false;
      this.retryConnection();
    });
  }

  connect() {3
    // Si ya está conectado, no se intenta reconectar
    if (this.isConnected) return;
    
    console.log('[TCPc] trying to connect TCPserver...');
    this.tcpClient.connect(C_SERVER_PORT, C_SERVER_HOST, () => {
      console.log('[TCPc] TCPserver connected');
      this.isConnected = true;
    });
  }

  retryConnection() {
    // Solo se reintenta si el cliente no está conectado
    if (!this.isConnected) {
      console.log('[TCPc] Trying reconnection...');
      setTimeout(() => {
        // Asegurarse de que el socket anterior se destruya para evitar conflictos
        this.tcpClient.destroy();
        this.tcpClient = new net.Socket();
        this.setupListeners();
        this.connect();
      }, RECONNECT_DELAY);
    }
  }
}

module.exports = TcpClient;
