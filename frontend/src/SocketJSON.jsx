// SocketJSON.jsx
import { useEffect } from 'react';
import { useSocket } from './SocketContext';

const SocketJSON = ({ onSocketData }) => {
  const socket = useSocket();

  useEffect(() => {
    if (!socket) return;

    const handleJsonData = (parsed) => {
      console.log("[Web] JSON data received:", parsed);
      
      if (parsed && parsed.data) {
        const { band, fmin, fmax, units, measure, vectors } = parsed.data;
        const { Pxx, f } = vectors;
        const data = [band, fmin, fmax, units, measure, Pxx, f];
        
        // Desestructuramos con valores por defecto
        const [
          bandValue = "N/A",
          fminValue = "N/A",
          fmaxValue = "N/A",
          unitsValue = "N/A",
          measureValue = "N/A",
          PxxValue = [],
          fValue = []
        ] = data;
        
        // Armamos el objeto con las variables
        const dataObj = {
          band: bandValue,
          fmin: fminValue,
          fmax: fmaxValue,
          units: unitsValue,
          measure: measureValue,
          Pxx: PxxValue,
          f: fValue
        };

        // Enviar el objeto al callback para que lo reciba App.jsx
        if (typeof onSocketData === 'function') {
          onSocketData(dataObj);
        }
      }
    };

    // Escuchar el evento "jsonData"
    socket.on("jsonData", handleJsonData);

    // Limpiar el listener al desmontar el componente
    return () => {
      socket.off("jsonData", handleJsonData);
    };
  }, [socket, onSocketData]);

  return null; // Este componente no renderiza nada en el DOM
};

export default SocketJSON;
