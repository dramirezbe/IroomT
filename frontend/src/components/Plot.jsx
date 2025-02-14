import React, { useEffect, useRef, useState } from 'react';
import Plotly from 'plotly.js-dist';

const PlotlyChart = ({ xData, yData }) => {
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
    const colorAccentLight = styles.getPropertyValue("--color-accent-light").trim();

    if (!xData || !yData || xData.length === 0 || yData.length === 0) return;

    const minX = Math.min(...xData);
    const maxX = Math.max(...xData);

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
        text: 'Gráfico de datos',
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
        linecolor: colorTextPrimary
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
      paper_bgcolor: 'rgba(0,0,0,0)',
      plot_bgcolor: 'rgba(0,0,0,0)',
      margin: { t: 40, b: 80, l: 80, r: 40 }
    };

    // Delay para asegurar que el DOM está listo
    const timer = setTimeout(() => {
      if (chartRef.current && isMounted) {
        Plotly.newPlot(chartRef.current, data, layout);
      }
    }, 50);

    return () => {
      clearTimeout(timer);
      if (chartRef.current && isMounted) {
        Plotly.purge(chartRef.current);
      }
    };
  }, [xData, yData, isMounted]);

  return <div ref={chartRef} style={{ 
    height: "500px", 
    width: "100%",
    backgroundColor: 'var(--color-background)'
  }} />;
};

export default PlotlyChart;