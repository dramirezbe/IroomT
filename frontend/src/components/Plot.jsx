import React, { useEffect, useRef, useState } from 'react';
import Plotly from 'plotly.js-dist';

import "./Plot.css"

const PlotlyChart = ({ xData, yData }) => {
  const minX = Math.min(...xData);
  const maxX = Math.max(...xData);
  const chartRef = useRef(null);
  const [isMounted, setIsMounted] = useState(false);

  useEffect(() => {
    setIsMounted(true);
    return () => setIsMounted(false);
  }, []);

  useEffect(() => {
    if (!isMounted || !chartRef.current) return;

    const styles = getComputedStyle(document.documentElement);
    const colorTextPrimary = styles.getPropertyValue("--color-text-primary").trim();
    const colorAccent = styles.getPropertyValue("--color-accent").trim();

    if (!xData || !yData || xData.length === 0 || yData.length === 0) return;

    const data = [{
      x: xData,
      y: yData,
      type: 'scatter',
      mode: 'lines',
      line: {
        color: colorAccent,
        width: 2
      },
      name: 'Spectrum'
    }];

    const layout = {
      title: {
        text: 'Spectrum',
        font: {
          color: colorTextPrimary,
          size: 16
        }
      },
      xaxis: {
        title: {
          text: 'Frequency (Hz)',
          font: {
            color: colorTextPrimary,
            size: 14
          }
        },
        range: [minX, maxX],
        tickfont: { color: colorTextPrimary },
        gridcolor: `${colorTextPrimary}30`,
        linecolor: colorTextPrimary,
        showline: true,   // Forzamos la visualización de la línea del eje
        mirror: false,    // Evitamos que se dibuje la línea opuesta
        zeroline: false   // Desactivamos la línea del cero
      },
      yaxis: {
        title: {
          text: 'Magnitude (dB)',
          font: {
            color: colorTextPrimary,
            size: 14
          }
        },
        tickfont: { color: colorTextPrimary },
        gridcolor: `${colorTextPrimary}30`,
        linecolor: colorTextPrimary
      },
      paper_bgcolor: 'rgba(0,0,0,0)', // Fondo transparente
      plot_bgcolor: 'rgba(0,0,0,0)',  // Fondo transparente
      margin: { t: 40, b: 80, l: 80, r: 40 },
      autosize: true // Asegura que el gráfico se ajuste al contenedor
    };

    if (chartRef.current.data) {
      Plotly.react(chartRef.current, data, layout);
    } else {
      Plotly.newPlot(chartRef.current, data, layout);
    }

    // Redimensionar el gráfico cuando el contenedor cambie de tamaño
    const handleResize = () => {
      Plotly.Plots.resize(chartRef.current);
    };

    window.addEventListener('resize', handleResize);

    return () => {
      if (chartRef.current && isMounted) {
        Plotly.purge(chartRef.current);
      }
      window.removeEventListener('resize', handleResize);
    };
  }, [xData, yData, isMounted]);

  return (
    <div
      ref={chartRef} 
      className='plot-container'
    />
  );
};

export default PlotlyChart;