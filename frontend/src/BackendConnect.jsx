import React, { useState, useEffect, useCallback } from 'react';
import { io } from 'socket.io-client';
import PlotlyChart from "./components/Plot";

import "./BackendConnect.css"

const socket = io('http://localhost:3001');

function RealTimePlot() {
  const [isStreaming, setIsStreaming] = useState(false);
  const [data, setData] = useState({ xData: [], yData: [] });

  // Manejar datos entrantes
  useEffect(() => {
    socket.on('data', (newData) => {
      setData({
        xData: newData.map(point => point.x),
        yData: newData.map(point => point.y)
      });
    });

    return () => {
      socket.off('data');
    };
  }, []);

  // Control del streaming
  const handleStart = useCallback(() => {
    if (!isStreaming) {
      setIsStreaming(true);
      socket.emit('start');
    }
  }, [isStreaming]);

  const handleStop = useCallback(() => {
    if (isStreaming) {
      setIsStreaming(false);
      socket.emit('stop');
    }
  }, [isStreaming]);

  return (
    <div className='backend-container'>
      <div>
        <button 
          onClick={handleStart}
        >
          Iniciar
        </button>
        <button 
          onClick={handleStop}
        >
          Detener
        </button>
      </div>
      <PlotlyChart xData={data.xData} yData={data.yData} />
    </div>
  );
}

export default RealTimePlot;