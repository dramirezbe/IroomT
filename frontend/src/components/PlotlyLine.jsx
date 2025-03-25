import React, { useEffect, useRef, useState } from 'react';
import Plotly from 'plotly.js-dist';


const PlotlyLine = ({ xData = [], yData = [] }) => {
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

    // Verificar si hay datos
    const hasData = xData.length > 0 && yData.length > 0;
    // En caso de no tener datos, usamos un rango por defecto para el eje x
    const minX = hasData ? Math.min(...xData) : 0;
    const maxX = hasData ? Math.max(...xData) : 1;

    // Si no hay datos, la traza es un array vacío para que solo se muestre el layout
    const data = hasData ? [{
      x: xData,
      y: yData,
      type: 'scatter',
      mode: 'lines',
      line: {
        color: colorAccent,
        width: 2
      },
      name: 'Spectrum'
    }] : [];

    const layout = {
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
        showline: true,
        mirror: false,
        zeroline: false,
        showgrid: true
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
        linecolor: colorTextPrimary,
        showgrid: true
      },
      paper_bgcolor: 'rgba(0,0,0,0)',
      plot_bgcolor: 'rgba(0,0,0,0)',
      margin: { t: 40, b: 80, l: 80, r: 40 },
      autosize: true
    };

    // Si ya existe una instancia, se actualiza, sino se crea desde cero
    if (chartRef.current.data) {
      Plotly.react(chartRef.current, data, layout);
    } else {
      Plotly.newPlot(chartRef.current, data, layout);
    }

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
    <div ref={chartRef} style={{marginTop: -25, marginLeft: -20}}/>
  );
};

export default PlotlyLine;
