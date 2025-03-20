// App.jsx
import React, { useState } from 'react';
import { SocketProvider } from './SocketContext';
import SocketJSON from './SocketJSON';
import Header from './components/Header';
import PlotlyC from './components/PlotlyC';
import InfoPlot from './components/InfoPlot';
import Heatmap from './components/Heatmap';

function App() {
  // Estado inicial con valores por defecto
  const [socketData, setSocketData] = useState({
    band: "N/A",
    fmin: "N/A",
    fmax: "N/A",
    units: "N/A",
    measure: "N/A",
    Pxx: [],
    f: []
  });

  const handleSocketData = (dataObj) => {
    console.log("Datos recibidos en App:", dataObj);
    setSocketData(dataObj);
  };

  // Desestructurar los datos recibidos para pasarlos a los componentes
  const { band, fmin, fmax, units, measure, Pxx, f } = socketData;

  return (
    <SocketProvider>
      <React.Fragment>
        <Header />
        <main className="main-container">
          <section className="top-container">
            <PlotlyC xData={f} yData={Pxx} />
            <InfoPlot band={band} fmin={fmin} fmax={fmax} units={units} measure={measure} />
          </section>
          <section className="bottom-container">
            <Heatmap />
            {/* Aquí puedes pasar la instancia del socket a UserControls si es necesario */}
          </section>
        </main>
        {/* Componente que se encarga de recibir el JSON vía socket */}
        <SocketJSON onSocketData={handleSocketData} />
      </React.Fragment>
    </SocketProvider>
  );
}

export default App;
