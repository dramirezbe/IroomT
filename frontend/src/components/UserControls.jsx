// UserControls.jsx
import React, { useState, useCallback } from 'react';
import "./UserControls.css"

const UserControls = ({ socket, onFrequenciesChange }) => {
  const [isStreaming, setIsStreaming] = useState(false);
  const [frecuencia1, setFrecuencia1] = useState('');
  const [frecuencia2, setFrecuencia2] = useState('');

  const handleStart = useCallback(() => {
    if (!isStreaming && frecuencia1 && frecuencia2 && socket) {
      setIsStreaming(true);
      const frequencies = [parseFloat(frecuencia1), parseFloat(frecuencia2)];
      socket.emit('start', frequencies);
      onFrequenciesChange(frequencies);
    }
  }, [isStreaming, frecuencia1, frecuencia2, socket, onFrequenciesChange]);

  const handleStop = useCallback(() => {
    if (isStreaming && socket) {
      setIsStreaming(false);
      socket.emit('stop');
    }
  }, [isStreaming, socket]);

  return (
    <div className="user-controls-container">
      <div className="input-group">
        <label htmlFor="frecuencia1">Frecuencia 1 (Hz):</label>
        <input
          type="number"
          id="frecuencia1"
          placeholder="Ej: 1000"
          value={frecuencia1}
          onChange={(e) => setFrecuencia1(e.target.value)}
        />
      </div>
      <div className="input-group">
        <label htmlFor="frecuencia2">Frecuencia 2 (Hz):</label>
        <input
          type="number"
          id="frecuencia2"
          placeholder="Ej: 5000"
          value={frecuencia2}
          onChange={(e) => setFrecuencia2(e.target.value)}
        />
      </div>
      <div className="button-group">
        <button id="iniciarBtn" onClick={handleStart} disabled={!frecuencia1 || !frecuencia2}>
          Iniciar
        </button>
        <button id="detenerBtn" onClick={handleStop} disabled={!isStreaming}>
          Detener
        </button>
      </div>
    </div>
  );
};

export default UserControls;
