import { connect } from 'mqtt';

// Conexión al broker usando protocolo MQTT (TCP) en el puerto 1883
const client = connect('mqtt://localhost:1883');

client.on('connect', () => {
  console.log('Conectado al broker MQTT');
  
  // Variable a enviar
  const variable = {
    data: "Hello World",
    timestamp: new Date()
  };
  
  // Publica el mensaje en el topic 'test/variable' con el flag 'retain'
  client.publish('test/variable', JSON.stringify(variable), { retain: true }, (err) => {
    if (err) {
      console.error('Error al publicar:', err);
    } else {
      console.log('Mensaje publicado correctamente');
    }
    client.end();
  });
});
