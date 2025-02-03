import React from "react";
import Plot from "./assets/Plot"; // Ajusta la ruta según tu estructura de archivos
import "./App.css";

// Generador de número aleatorio entre 1 y numLim
const randomNumber = (numLim: number): number => {
  return Math.floor(Math.random() * numLim) + 1;
};

// Generador de ruido aleatorio basado en diferentes funciones
const randomNoise = (xData: number[], noiseStdDev: number, ranNum: number): number[] => {
  switch (ranNum) {
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
};

const App: React.FC = () => {
  // Datos de prueba
  const xData: number[] = Array.from({ length: 4096 }, (_, i) => i); // Genera valores de 0 a 4095
  const ranNum = randomNumber(4); // Genera un número aleatorio entre 1 y 4
  const yData: number[] = randomNoise(xData, 0.0001, ranNum); // Genera ruido aleatorio

  console.log(xData.length)


  return (
    <div className="main-container">
      <div className="top-container">
        <div className="Plot-div">
          <h1> RF Spectrum Monitor</h1>
          <Plot xData={xData} yData={yData} />
          <p>randomNumber: {ranNum}</p>
        </div>

        <div className="Analytics">
          <h1>Analytics</h1>
          <ul aria-label="Métricas del espectro">
            <li>SNR (dB): #</li>
            <li>Electromagnetic energy: #</li>
            <li>RMS: #</li>
            <li>Central-Frequency: #</li>
            <li>Power: #</li>
            <li>Power Max: #</li>
            <li>SNR: #</li>
            <li>Presence: #</li>
          </ul>
        </div>
      </div>
      <div className="Reserve-Container"> 
        <h1>RESERVED BOX</h1>
      </div>
    </div>
  );
};

export default App;
