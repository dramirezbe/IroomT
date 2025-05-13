// SocketJSON.jsx
import { useEffect } from 'react';
import { useSocket } from './SocketContext';

/**
 * 
 * SocketJSON React component hook
 * 
 * This component listens for incoming JSON data on a WebSocket connection
 * provided by SocketContext. When JSON data arrives via the "jsonData"
 * event, it parses and destructures the payload, applies default values,
 * and forwards a well-structured object to the parent via the onSocketData callback.
 *
 * @param {{ onSocketData: (data: {
 *   band: string | number,
 *   fmin: string | number,
 *   fmax: string | number,
 *   units: string,
 *   measure: string,
 *   Pxx: number[],
 *   f: number[]
 * }) => void }} props - Component props
 * @returns {null} Does not render any DOM elements
 */
const SocketJSON = ({ onSocketData }) => {
  // Acquire socket instance from context
  const socket = useSocket();

  useEffect(() => {
    if (!socket) return; // Exit if socket is not yet initialized

    /**
     * Handles incoming parsed JSON data from the socket event
     * @param {{ data?: { band?: any, fmin?: any, fmax?: any, units?: any, measure?: any, vectors?: { Pxx?: number[], f?: number[] } } }} parsed
     */
    const handleJsonData = (parsed) => {
      console.log('[Web] JSON data received:', parsed);

      if (parsed && parsed.data) {
        // Destructure data payload
        const { band, fmin, fmax, units, measure, vectors } = parsed.data;
        const { Pxx, f } = vectors;

        // Combine into an array for safe destructuring with defaults
        const data = [band, fmin, fmax, units, measure, Pxx, f];

        // Apply default values if any field is undefined
        const [
          bandValue = 'N/A',
          fminValue = 'N/A',
          fmaxValue = 'N/A',
          unitsValue = 'N/A',
          measureValue = 'N/A',
          PxxValue = [],
          fValue = []
        ] = data;

        // Build final data object
        const dataObj = {
          band: bandValue,
          fmin: fminValue,
          fmax: fmaxValue,
          units: unitsValue,
          measure: measureValue,
          Pxx: PxxValue,
          f: fValue
        };

        // Invoke callback if provided
        if (typeof onSocketData === 'function') {
          onSocketData(dataObj);
        }
      }
    };

    // Listen for the custom "jsonData" event on the socket
    socket.on('jsonData', handleJsonData);

    // Cleanup listener when component unmounts or dependencies change
    return () => {
      socket.off('jsonData', handleJsonData);
    };
  }, [socket, onSocketData]);

  // This component does not render any visual elements
  return null;
};

export default SocketJSON;
