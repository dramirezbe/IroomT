// App.jsx
import React, { useState } from 'react';
import { SocketProvider } from './SocketContext';
import SocketJSON from './SocketJSON';
import Header from './components/Header';
import PlotlyLine from './components/PlotlyLine';
import InfoPlot from './components/InfoPlot';
import PlotlyHeat from './components/PlotlyHeat';

import './App.css';

/**
 * Main application component.
 * Wraps content in SocketProvider to supply WebSocket connection.
 * Renders header, spectrum plot, info panel, heatmap, and logo.
 */
function App() {
  /**
   * Local state for incoming socket data, initialized with defaults.
   * @typedef {Object} SocketData
   * @property {string|number} band - Frequency band identifier
   * @property {string|number} fmin - Minimum frequency value
   * @property {string|number} fmax - Maximum frequency value
   * @property {string} units - Measurement units (e.g., 'dB')
   * @property {string} measure - Description of the measurement
   * @property {number[]} Pxx - Power spectral density values
   * @property {number[]} f - Frequency bin values
   */
  const [socketData, setSocketData] = useState({
    band: 'N/A',
    fmin: 'N/A',
    fmax: 'N/A',
    units: 'N/A',
    measure: 'N/A',
    Pxx: [],
    f: []
  });

  /**
   * Callback passed to SocketJSON to receive parsed socket data.
   * Updates local state when new data arrives.
   * @param {SocketData} dataObj - Structured data from socket
   */
  const handleSocketData = (dataObj) => {
    console.log('[Web] Data received in App:', dataObj);
    setSocketData(dataObj);
  };

  // Destructure socket data for easy prop passing
  const { band, fmin, fmax, units, measure, Pxx, f } = socketData;

  return (
    <SocketProvider>
      {/* Fragment used to group top-level elements */}
      <>
        {/* Static header component */}
        <Header />

        {/* Main content container */}
        <main className="main-container">
          {/* Top section: line plot and info panel */}
          <section className="top-container">
            <div className="plot-container">
              <h1>Spectrum</h1>
              {/* Line chart plotting frequency vs. PSD */}
              <PlotlyLine xData={f} yData={Pxx} />
            </div>
            <div className="info-container">
              {/* Panel displaying metadata about current plot */}
              <InfoPlot
                band={band}
                fmin={fmin}
                fmax={fmax}
                units={units}
                measure={measure}
              />
            </div>
          </section>

          {/* Bottom section: heatmap and branding */}
          <section className="bottom-container">
            <div className="heatmap-container">
              {/* Heatmap component for visualizing temporal data */}
              <PlotlyHeat />
            </div>
            <div className="user-container">
              {/* Company logo image */}
              <img
                className="logo"
                src="Logo_GCPDS_english.svg"
                alt="GCPDS Logo"
              />
            </div>
          </section>
        </main>

        {/* Invisible component that handles WebSocket data events */}
        <SocketJSON onSocketData={handleSocketData} />
      </>
    </SocketProvider>
  );
}

export default App;
