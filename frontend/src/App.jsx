// App.jsx
import React, { useState, useEffect } from "react";
import Heatmap from "./components/Heatmap";
import Header from "./components/Header";
import InfoPlot from "./components/InfoPlot";
import PlotlyC from "./components/PlotlyC";

import { useSocket } from './SocketContext';



import "./App.css";

const App = () => {
  const socket = useSocket();
  const [socketData, setSocketData] = useState([]);

  useEffect(() => {
    // Escuchar el evento con los datos del servidor TCP
    socket.on("tcp-data", (data) => {
      console.log("Datos recibidos:", data);
      // Se asume que data ya es un array en el formato:
      // [band, fmin, fmax, units, measure, Pxx, f]
      setSocketData(data);
    });

    // Limpieza de la conexión al desmontar el componente
    return () => socket.disconnect();
  }, [socket]);

  // Desestructuramos el array recibido, asignando valores por defecto en caso de que aún no se hayan recibido datos
  const [
    band = "N/A",
    fmin = "N/A",
    fmax = "N/A",
    units = "N/A",
    measure = "N/A",
    Pxx = [],
    f = []
  ] = socketData;

  return (
    <React.Fragment>
      <Header />
      <main className="main-container">
        <section className="top-container">
          <PlotlyC xData={f} yData={Pxx} />
          <InfoPlot band={band} fmin={fmin} fmax={fmax} units={units} measure={measure}/>
        </section>
        <section className="bottom-container">
          <Heatmap />
          {/* Pasa la instancia de socket a UserControls */}
        </section>
      </main>
    </React.Fragment>
  );
};

export default App;
