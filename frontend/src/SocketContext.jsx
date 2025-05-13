// SocketContext.jsx
import React, { createContext, useContext, useEffect, useState } from 'react';
import io from 'socket.io-client';

/**
 * URL of the Socket.IO server, constructed from environment variables.
 * VITE_SERVER_IP   - host IP or domain where the socket server is running
 * VITE_BUILD_PORT  - port number exposed by the server
 */
const SOCKET_SERVER_URL = `http://${import.meta.env.VITE_SERVER_IP}:${import.meta.env.VITE_BUILD_PORT}`;

/**
 * React Context to provide a Socket.IO client instance throughout the app.
 * @type {React.Context<import('socket.io-client').Socket|null>}
 */
const SocketContext = createContext(null);

/**
 * SocketProvider component wraps the application tree and
 * initializes a Socket.IO client connection when mounted.
 * It passes the socket instance down via context.
 *
 * @param {{ children: React.ReactNode }} props
 * @returns {JSX.Element} Context provider with socket value
 */
export const SocketProvider = ({ children }) => {
  // Local state to hold the socket instance
  const [socket, setSocket] = useState(null);

  useEffect(() => {
    // Create new Socket.IO client, forcing websocket transport
    const newSocket = io(SOCKET_SERVER_URL, { transports: ['websocket'] });
    setSocket(newSocket);

    // On cleanup (unmount), close the socket connection
    return () => {
      newSocket.close();
    };
  }, []);

  return (
    <SocketContext.Provider value={socket}>
      {children}
    </SocketContext.Provider>
  );
};

/**
 * Custom hook to consume the SocketContext and obtain the socket instance.
 * @returns {import('socket.io-client').Socket|null} The Socket.IO client or null if not initialized
 */
export const useSocket = () => useContext(SocketContext);
