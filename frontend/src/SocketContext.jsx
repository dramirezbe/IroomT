import React, { createContext, useContext, useEffect, useState } from 'react';
import io from 'socket.io-client';

// URL del servidor o configuración del socket
const SOCKET_SERVER_URL = 'http://localhost:3001';

const SocketContext = createContext(null);

export const SocketProvider = ({ children }) => {
  const [socket, setSocket] = useState(null);

  useEffect(() => {
    // Inicializamos el socket
    const newSocket = io(SOCKET_SERVER_URL, { transports: ['websocket'] });
    setSocket(newSocket);

    // Limpieza cuando se desmonte el componente
    return () => newSocket.close();
  }, []);

  return (
    <SocketContext.Provider value={socket}>
      {children}
    </SocketContext.Provider>
  );
};

// Hook para usar el contexto del socket en otros componentes
export const useSocket = () => {
  return useContext(SocketContext);
};
