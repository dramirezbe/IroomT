import React from "react";
import {
  Chart as ChartJS,
  LineElement,
  PointElement,
  LinearScale,
  CategoryScale,
  Tooltip,
  Legend,
} from "chart.js";
import { Line } from "react-chartjs-2";

import "../index.css";
import './Plot.css';

// Obtén las variables de color definidas en el CSS
const styles = getComputedStyle(document.documentElement);
const colorTextPrimary = styles.getPropertyValue("--color-text-primary").trim();
const colorAccent = styles.getPropertyValue("--color-accent").trim();
const colorAccentLight = styles.getPropertyValue("--color-accent-light").trim();

// Registrar componentes de Chart.js
ChartJS.register(LineElement, PointElement, LinearScale, CategoryScale, Tooltip, Legend);

interface PlotProps {
  xData: number[];
  yData: number[];
}

const Plot: React.FC<PlotProps> = ({ xData, yData }) => {
  if (!xData || !yData || xData.length !== yData.length || xData.length === 0) {
    return <div>Error: xData and yData must have the same non-zero length.</div>;
  }

  // Datos del gráfico
  const data = {
    labels: xData,
    datasets: [
      {
        label: "Spectrum",
        data: yData,
        borderColor: colorAccent,
        backgroundColor: colorAccentLight,
        borderWidth: 2,
        pointRadius: 0, // Evitar puntos en conjuntos grandes
        tension: 0.1, // Curva suavizada
      },
    ],
  };

  // Opciones del gráfico
  const options = {
    responsive: true,
    maintainAspectRatio: false,
    scales: {
      x: {
        type: "linear" as const,
        min: Math.min(...xData),
        max: Math.max(...xData),
        title: {
          display: true,
          text: "Frequency (Hz)",
          color: colorTextPrimary,
          font: { size: 14 },
        },
        ticks: { color: colorTextPrimary },
        grid: { color: colorTextPrimary },
      },
      y: {
        title: {
          display: true,
          text: "Magnitude (dB)",
          color: colorTextPrimary,
          font: { size: 14 },
        },
        ticks: { color: colorTextPrimary },
        grid: { color: colorTextPrimary },
      },
    },
    plugins: {
      legend: {
        display: true,
        position: "top",
        labels: { color: colorTextPrimary },
      },
    },
    animation: true,
  };

  return (
    <div className="plot-container">
      <Line data={data} options={options} />
    </div>
  );
};

export default Plot;
