const net = require('net');
const readJSONCallback = require('./handleJSON');

const C_SERVER_HOST = 'localhost';
const C_SERVER_PORT = 8080;

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
          console.log('[TCP] JSON ya fue creado en C_SERVER');
          // Ejecutar el callback para leer el JSON
          readJSONCallback((err, socketData) => {
            if (err) {
              console.error("Hubo un error al leer el JSON:", err);
            } else {
              console.log("Datos leídos:", socketData);
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
      console.error('[TCP] Error:', err.message);
      this.isConnected = false;
    });

    this.tcpClient.on('close', () => {
      console.log('[TCP] Conexión cerrada');
      this.isConnected = false;
    });
  }

  connect() {
    this.tcpClient.connect(C_SERVER_PORT, C_SERVER_HOST, () => {
      console.log('[TCP] Conectado a C_SERVER');
      this.isConnected = true;
    });
  }
}

module.exports = TcpClient;
