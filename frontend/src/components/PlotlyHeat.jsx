import React, { useEffect, useRef } from "react";
import Plotly from "plotly.js-dist"; // Asegúrate de tener instalada esta dependencia

import "../index.css"; // Importar estilos globales

const HEATMAP_SIZE = 10;

const PlotlyHeat = () => {
  const divRef = useRef(null);

  useEffect(() => {
    
    const styles = getComputedStyle(document.documentElement);
    const colorTextPrimary = styles.getPropertyValue("--color-text-primary").trim();
    const colorAccent = styles.getPropertyValue("--color-accent").trim();
    const colorPrimary = styles.getPropertyValue("--color-primary").trim();
    const colorSecondary = styles.getPropertyValue("--color-secondary").trim();
    
    const intenseZero = styles.getPropertyValue("--intensity-0").trim();
    const intenseOne = styles.getPropertyValue("--intensity-1").trim();
    const intenseTwo = styles.getPropertyValue("--intensity-2").trim();
    const intenseThree = styles.getPropertyValue("--intensity-3").trim();
    const intenseFour = styles.getPropertyValue("--intensity-4").trim();
    

    // Función para generar una matriz de 20x20 con números aleatorios entre 0 y 100
    const generateRandomData = () => {
      const data = [];
      for (let i = 0; i < HEATMAP_SIZE; i++) {
        const row = [];
        for (let j = 0; j < HEATMAP_SIZE; j++) {
          row.push(Math.random() * 100);
        }
        data.push(row);
      }
      return data;
    };

    const randomData = generateRandomData();

    // Configuración del heatmap con la paleta de colores personalizada:
    // verde para poca potencia, amarillo para potencia media, y rojo para mucha potencia
    const data = [
      {
        z: randomData,
        type: "heatmap",
        colorscale: [
          [0, intenseZero],    // Valor mínimo: poca potencia
          [0.25, intenseOne], // Valor medio: potencia media
          [0.5, intenseTwo],
          [0.75, intenseThree],
          [1, intenseFour],       // Valor máximo: mucha potencia
        ],
      },
    ];

    // Layout con configuración para modificar el fondo del gráfico
    const layout = {
      paper_bgcolor: colorPrimary, // Color de fondo general de la figura
    };

    Plotly.newPlot(divRef.current, data, layout);
  }, []);

  return <div ref={divRef} />;
};

export default PlotlyHeat;
