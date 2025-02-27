const net = require('net');

const C_SERVER_HOST = 'localhost';
const C_SERVER_PORT = 8080;
const DATA_POINTS = 4096;
const POINT_SIZE = 16; // 8 bytes (x) + 8 bytes (y)
const EXPECTED_BYTES = DATA_POINTS * POINT_SIZE;

class TcpClient {
  constructor() {
    this.tcpClient = new net.Socket();
    this.isConnected = false;
    this.receivedData = Buffer.alloc(0);
    this.onData = null; // Callback para emitir los datos a Socket.io
    this.setupListeners();
  }

  setupListeners() {
    this.tcpClient.on('data', (chunk) => {
      this.receivedData = Buffer.concat([this.receivedData, chunk]);
      if (this.receivedData.length >= EXPECTED_BYTES) {
        try {
          const points = [];
          const validData = this.receivedData.subarray(0, EXPECTED_BYTES);
          for (let i = 0; i < DATA_POINTS; i++) {
            const offset = i * POINT_SIZE;
            const x = validData.readDoubleLE(offset);
            const y = validData.readDoubleLE(offset + 8);
            points.push({ x, y });
          }
          // Llamamos al callback si está definido
          this.onData && this.onData(points);
          // Eliminamos la parte procesada
          this.receivedData = this.receivedData.subarray(EXPECTED_BYTES);
        } catch (err) {
          console.error('[TCP] Error procesando datos:', err.message);
        }
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

  connect(frequencies) {
    this.tcpClient.connect(C_SERVER_PORT, C_SERVER_HOST, () => {
      console.log('[TCP] Conectado a C de forma persistente');
      // Envía las frecuencias iniciales
      const freqBuffer = Buffer.alloc(16);
      freqBuffer.writeDoubleLE(frequencies[0], 0);
      freqBuffer.writeDoubleLE(frequencies[1], 8);
      this.tcpClient.write(freqBuffer);
      this.isConnected = true;
    });
  }

  updateFrequencies(frequencies) {
    if (this.isConnected) {
      const updateBuffer = Buffer.alloc(20); // 4 bytes comando + 16 bytes frecuencias
      updateBuffer.write("FREQ", 0, 4, 'utf8');
      updateBuffer.writeDoubleLE(frequencies[0], 4);
      updateBuffer.writeDoubleLE(frequencies[1], 12);
      this.tcpClient.write(updateBuffer);
    }
  }

  requestData() {
    if (this.isConnected) {
      this.tcpClient.write(Buffer.from("REQ"));
    }
  }

  stop() {
    if (this.isConnected) {
      this.tcpClient.write(Buffer.from("STOP"));
    }
  }
}

module.exports = TcpClient;
