import React, { useEffect, useRef } from "react";
import L, { Map } from "leaflet";
import "leaflet.heat";

import "./Heatmap.css";

const Heatmap: React.FC = () => {
  // Usamos useRef para referenciar el contenedor del mapa y almacenar la instancia del mapa.
  const mapContainerRef = useRef<HTMLDivElement | null>(null);
  const mapRef = useRef<Map | null>(null);

  useEffect(() => {
    // Verifica que el contenedor exista antes de inicializar el mapa.
    if (!mapContainerRef.current) return;

    // Inicializa el mapa usando el contenedor referenciado.
    const map = L.map(mapContainerRef.current, {
      crs: L.CRS.Simple, // Utiliza coordenadas en píxeles en lugar de lat/lng.
      minZoom: -2,       // Permite hacer zoom hacia adentro.
    }).setView([200, 200], 1); // Centra la vista en [200,200] con un zoom inicial de 1.

    // Guarda la instancia del mapa en el ref.
    mapRef.current = map;

    // Define los límites de la "habitación" (área de 400x400 píxeles).
    const bounds: L.LatLngBoundsExpression = [
      [0, 0],     // Esquina superior izquierda.
      [400, 400], // Esquina inferior derecha.
    ];

    // Agrega una imagen superpuesta; actualmente sin URL, pero se puede sustituir por una imagen de fondo.
    L.imageOverlay("", bounds).addTo(map);

    // Ajusta el mapa para que se enfoque en los límites definidos.
    map.fitBounds(bounds);

    // Genera puntos aleatorios para el heatmap, cada punto contiene:
    // [posición x, posición y, intensidad (entre 0 y 1)].
    const heatPoints: [number, number, number][] = Array.from({ length: 50 }, () => [
      Math.random() * 400, // x aleatorio entre 0 y 400.
      Math.random() * 400, // y aleatorio entre 0 y 400.
      Math.random(),       // Intensidad aleatoria.
    ]);

    // Crea y agrega la capa de heatmap al mapa.
    L.heatLayer(heatPoints, { radius: 25, blur: 15, maxZoom: 1 }).addTo(map);

    // Limpieza: remueve el mapa al desmontar el componente.
    return () => {
      map.remove();
    };
  }, []);

  return <div ref={mapContainerRef} className="heatmap-container"></div>;
};

export default Heatmap;
