import React, { useEffect, useRef } from "react";
import L, { Map } from "leaflet";
import "leaflet.heat";

import "./Heatmap.css";
import '../index.css'; // Importar estilos globales

const Heatmap: React.FC = () => {
  const mapContainerRef = useRef<HTMLDivElement | null>(null);
  const mapRef = useRef<Map | null>(null);

  useEffect(() => {
    if (!mapContainerRef.current) return;

    // Inicializa el mapa.
    const map = L.map(mapContainerRef.current, {
      crs: L.CRS.Simple, // Usa coordenadas en píxeles.
      minZoom: -2,       // Permite hacer zoom hacia adentro.
    }).setView([127.75, 183.75], 0); // Centra la vista en el centro de la imagen reducida (255.5/2, 367.5/2).

    mapRef.current = map;

    // Define los límites del mapa para que la imagen se muestre más pequeña.
    const bounds: L.LatLngBoundsExpression = [
      [0, 0],           // Esquina superior izquierda.
      [255.5, 367.5],   // Esquina inferior derecha (mitad del tamaño original).
    ];

    // Agrega la imagen superpuesta con los nuevos límites.
    L.imageOverlay("public/office-example.jpg", bounds).addTo(map);

    // Ajusta el mapa para que se enfoque en los límites definidos.
    map.fitBounds(bounds);

    // Genera puntos aleatorios para el heatmap dentro de los nuevos límites.
    const heatPoints: [number, number, number][] = Array.from({ length: 50 }, () => [
      Math.random() * 255.5, // x aleatorio entre 0 y 255.5 (altura reducida).
      Math.random() * 367.5, // y aleatorio entre 0 y 367.5 (ancho reducido).
      Math.random(),         // Intensidad aleatoria.
    ]);

    // Crea y agrega la capa de heatmap al mapa.
    L.heatLayer(heatPoints, { radius: 25, blur: 15, maxZoom: 1 }).addTo(map);

    // Limpieza: remueve el mapa al desmontar el componente.
    return () => {
      map.remove();
    };
  }, []);

  return (
    <div
      ref={mapContainerRef}
      className="heatmap-container"
      style={{ width: "100%", height: "511px" }} // Ajusta el contenedor a la altura original.
    ></div>
  );
};

export default Heatmap;