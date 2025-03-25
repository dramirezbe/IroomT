// App.jsx
import React, { useState } from 'react';
import { SocketProvider } from './SocketContext';
import SocketJSON from './SocketJSON';
import Header from './components/Header';
import PlotlyLine from './components/PlotlyLine';
import InfoPlot from './components/InfoPlot';
import PlotlyHeat from './components/PlotlyHeat';
import UserControl from './components/UserControl';

import './App.css';


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
            <div className='plot-container'>
              <h1>Spectrum</h1>
              <PlotlyLine xData={f} yData={Pxx} />
            </div>
            <div className='info-container'>
              <InfoPlot band={band} fmin={fmin} fmax={fmax} units={units} measure={measure} />
            </div>
          </section>
          <section className="bottom-container">
            <div className='heatmap-container'>
              <PlotlyHeat/>
            </div>
            <div className='user-container'>
              <UserControl/>
              <img 
                className="logo" 
                src="Logo_GCPDS_english.svg" 
                alt="GCPDS Logo" 
              />
            </div>
          </section>
        </main>
        <SocketJSON onSocketData={handleSocketData} />
      </React.Fragment>
    </SocketProvider>
  );
}

export default App;
