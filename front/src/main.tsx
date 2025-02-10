import { StrictMode } from "react"; //Ayuda a detectar problemas en la aplicación
import { createRoot } from "react-dom/client"; //Método de React 18 para renderizar la aplicación mediante DOM
import App from "./App"; //Componente App, contiene la estructura de la aplicación

const rootElement = document.getElementById("root");

if (rootElement) {
  createRoot(rootElement).render(
    <StrictMode>
      <App />
    </StrictMode>
  );
} else {
  console.error("No se encontró el elemento 'root' en el DOM.");
}
