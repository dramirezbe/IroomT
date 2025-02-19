// App.jsx
import React, { useState, useEffect } from "react";
import Heatmap from "./components/Heatmap";
import Header from "./components/Header";
import InfoPlot from "./components/InfoPlot";
import UserControls from "./components/UserControls";
import PlotlyC from "./components/PlotlyC";
import { io } from 'socket.io-client';
import "./App.css";

const App = () => {
  const [data, setData] = useState({ xData: [], yData: [] });
  const [socket, setSocket] = useState(null);

  useEffect(() => {
    const newSocket = io('http://localhost:3001', { transports: ['websocket'] });
    setSocket(newSocket);

    newSocket.on('data', (newData) => {
      setData({
        xData: newData.map(point => point.x),
        yData: newData.map(point => point.y)
      });
    });

    return () => newSocket.disconnect();
  }, []);

  const handleFrequenciesChange = (frequencies) => {
    console.log("Frecuencias actualizadas:", frequencies);
  };

  return (
    <React.Fragment>
      <Header />
      <main className="main-container">
        <section className="top-container">
          <PlotlyC xData={data.xData} yData={data.yData} />
          <InfoPlot />
        </section>
        <section className="bottom-container">
          <Heatmap />
          {/* Pasa la instancia de socket a UserControls */}
          <UserControls socket={socket} onFrequenciesChange={handleFrequenciesChange} />
        </section>
      </main>
    </React.Fragment>
  );
};

export default App;
