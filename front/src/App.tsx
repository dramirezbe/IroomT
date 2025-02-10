import React from "react";
import Plot from "./components/Plot.tsx";
import Heatmap from "./components/Heatmap.tsx";
import Header from "./components/Header.tsx";
import InfoPlot from "./components/InfoPlot.tsx";
import "./App.css";


// Generador de ruido aleatorio basado en diferentes funciones
const randomNoise = (xData: number[], noiseStdDev: number): number[] => {
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
};

const App: React.FC = () => {
  // Datos de prueba
  const xData: number[] = Array.from({ length: 4096 }, (_, i) => i); // Genera valores de 0 a 4095
  const yData: number[] = randomNoise(xData, 1e-7); // Genera ruido aleatorio

  return (
    <>
      <Header />
      <main className="main-container">
        <section className="top-container">
          <Plot xData={xData} yData={yData} />
          <InfoPlot />
        </section>
        <section className="bottom-container"> 
          <Heatmap />
        </section>
      </main>
    </>
  );
};

export default App;
