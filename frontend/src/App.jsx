import React from "react";
import Heatmap from "./components/Heatmap";
import Header from "./components/Header";
import InfoPlot from "./components/InfoPlot";

import RealTimePlot from "./BackendConnect";

import "./App.css";



// Generador de ruido aleatorio basado en diferentes funciones
/*const randomNoise = (xData, noiseStdDev) => {
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

const App = () => {
  // Datos de prueba
  //const xData = Array.from({ length: 4096 }, (_, i) => i); // Genera valores de 0 a 4095
  //const yData = randomNoise(xData, 1e-7); // Genera ruido aleatorio

  return (
    <React.Fragment>
      <Header />
      <main className="main-container">
        <section className="top-container">
          <RealTimePlot/>
          <InfoPlot />
        </section>
        <section className="bottom-container">
          <Heatmap />
        </section>
      </main>
    </React.Fragment>
  );
};

export default App;