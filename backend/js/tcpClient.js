const net = require('net');

const readJSONCallback = require('./handleJSON');

const C_SERVER_HOST = 'localhost';
const C_SERVER_PORT = 8080;

class TcpClient {
  constructor() {
    this.tcpClient = new net.Socket();
    this.isConnected = false;
    this.receivedData = Buffer.alloc(0);
    this.setupListeners();
  }

  setupListeners() {
    this.tcpClient.on('data', (chunk) => {
      this.receivedData = Buffer.concat([this.receivedData, chunk]);
      
      // Esperamos que se reciba al menos 1 byte para el booleano
      if (this.receivedData.length >= 1) {
        const flag = this.receivedData.readUInt8(0);
        
        if (flag === 1) {
          console.log('[TCP] JSON ya fue creado en C_SERVER');
          // Enviar comando para borrar el JSON
          readJSONCallback((err, socketData) => {
            if (err) {
              console.error("Hubo un error:", err);
            } else {
              console.log("Datos leídos:", socketData);
            }
          });
          
          this.tcpClient.write(Buffer.from("DEL"));
        }
        
        // Eliminamos el byte ya procesado
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
