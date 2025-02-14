import React, { useState, useEffect } from 'react';
import mqtt from 'mqtt';

const CounterComponent = () => {
  const [counter, setCounter] = useState(0);
  const [client, setClient] = useState(null);
  const [isConnected, setIsConnected] = useState(false);

  const handleStart = () => {
    if (!isConnected) {
      const mqttClient = mqtt.connect('ws://localhost:4883');

      mqttClient.on('connect', () => {
        setIsConnected(true);
        mqttClient.subscribe('counter/response', (err) => {
          if (!err) {
            mqttClient.publish('counter/request', '');
          }
        });
      });

      mqttClient.on('message', (topic, message) => {
        if (topic === 'counter/response') {
          setCounter(Number(message.toString()));
        }
      });

      mqttClient.on('error', (err) => {
        console.error('MQTT Error:', err);
      });

      setClient(mqttClient);
    } else {
      client.publish('counter/request', '');
    }
  };

  const handleStop = () => {
    if (client) {
      client.end(() => {
        setIsConnected(false);
        setClient(null);
        setCounter(0);
      });
    }
  };

  useEffect(() => {
    return () => {
      if (client) {
        client.end();
      }
    };
  }, [client]);

  return (
    <div>
      <button onClick={handleStart} disabled={isConnected}>
        Inicio
      </button>
      <button onClick={handleStop} disabled={!isConnected}>
        Final
      </button>
      <p>Contador: {counter}</p>
    </div>
  );
};

export default CounterComponent;