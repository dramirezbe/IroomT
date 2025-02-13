import React from "react";
import Plot from "./components/Plot.tsx";
import Heatmap from "./components/Heatmap.tsx";
import Header from "./components/Header.tsx";
import InfoPlot from "./components/InfoPlot.tsx";

//Server
import { MqttProvider, useSubscription } from 'mqtt-reactjs-hooks';

import "./App.css";

// Componente que se suscribe al topic 'test/variable'
const SubscriptionComponent: React.FC = () => {
  // useSubscription se encarga de la suscripción
  const { message, error } = useSubscription('test/variable');

  if (error) {
    return <div>Error al recibir el mensaje: {error.message}</div>;
  }

  if (!message) {
    return <div>No se ha recibido ningún mensaje.</div>;
  }

  let data;
  try {
    data = JSON.parse(message.payload.toString());
  } catch (err) {
    return <div>Error al parsear el mensaje.</div>;
  }

  return (
    <div>
      <h2>Mensaje recibido:</h2>
      <pre>{JSON.stringify(data, null, 2)}</pre>
    </div>
  );
};


// Generador de ruido aleatorio basado en diferentes funciones
/*const randomNoise = (xData: number[], noiseStdDev: number): number[] => {
  switch (Math.floor(Math.random() * 4) + 1) {
    case 1:
      return xData.map((x) => noiseStdDev * (Math.random() * 2 - 1 + Math.log10(x)));
    case 2:
      return xData.map((x) => noiseStdDev * (Math.random() * 2 - 1 + Math.sin(x)));
    case 3:
      return xData.map((x) => noiseStdDev * (Math.random() * 2 - 1 + Math.cos(x)));
    case 4:
      return xData.map((x) => noiseStdDev * (Math.random() * 2 - 1 + x ** 2));
    default:
      return xData.map((x) => noiseStdDev * (Math.random() * 2 - 1 + x));
  }
};*/



const App: React.FC = () => {
  // Datos de prueba
  //const xData: number[] = Array.from({ length: 4096 }, (_, i) => i); // Genera valores de 0 a 4095
  //const yData: number[] = randomNoise(xData, 1e-7); // Genera ruido aleatorio

  return (
    <>
    <MqttProvider url="ws://localhost:9001">
      <Header />
      <main className="main-container">
        <section className="top-container">
          <Plot xData={null} yData={null} />
          <div style={{ textAlign: 'center', marginTop: '2rem' }}>
            <h1>Aplicación MQTT con mqtt-reactjs-hooks</h1>
            <SubscriptionComponent />
          </div>
          <InfoPlot />
        </section>
        <section className="bottom-container"> 
          <Heatmap />
        </section>
      </main>
    </MqttProvider>
    </>
  );
};

export default App;
