import React, { useMemo } from "react";
import {
  Chart as ChartJS,
  LineElement,
  PointElement,
  LinearScale,
  CategoryScale,
  Tooltip,
  Legend,
  ChartOptions,
} from "chart.js";
import { Line } from "react-chartjs-2";

import "../index.css";
import "./Plot.css";

// Retrieve CSS variables from the root element
const styles = getComputedStyle(document.documentElement);
const colorTextPrimary = styles.getPropertyValue("--color-text-primary").trim();
const colorAccent = styles.getPropertyValue("--color-accent").trim();
const colorAccentLight = styles.getPropertyValue("--color-accent-light").trim();

// Register necessary Chart.js components
ChartJS.register(LineElement, PointElement, LinearScale, CategoryScale, Tooltip, Legend);

/**
 * Props for the Plot component.
 */
interface PlotProps {
  xData: number[];
  yData: number[];
}

/**
 * Plot component renders a line chart using react-chartjs-2.
 *
 * @param {number[]} xData - The x-axis data points.
 * @param {number[]} yData - The y-axis data points.
 *
 * @returns A React element containing the rendered chart or an error message if the data is invalid.
 */
const Plot: React.FC<PlotProps> = ({ xData, yData }) => {
  // Validate input data
  if (!xData || !yData || xData.length !== yData.length || xData.length === 0) {
    return <div>Error: xData and yData must have the same non-zero length.</div>;
  }

  // Compute the minimum and maximum values for the x-axis once per data change
  const [minX, maxX] = useMemo(() => {
    return [Math.min(...xData), Math.max(...xData)];
  }, [xData]);

  // Memoize chart data configuration to avoid unnecessary recalculations
  const data = useMemo(
    () => ({
      labels: xData,
      datasets: [
        {
          label: "Spectrum",
          data: yData,
          borderColor: colorAccent,
          backgroundColor: colorAccentLight,
          borderWidth: 2,
          pointRadius: 0,
          tension: 0.1,
        },
      ],
    }),
    [xData, yData]
  );

  // Memoize chart options configuration to avoid unnecessary recalculations
  const options = useMemo<ChartOptions<"line">>(
    () => ({
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        x: {
          type: "linear",
          min: minX,
          max: maxX,
          title: {
            display: true,
            text: "Frequency (Hz)",
            color: colorTextPrimary,
            font: { size: 14 },
          },
          ticks: { 
            color: colorTextPrimary,
            stepSize: 100  // Muestra una marca cada 100 unidades
          },
          grid: { color: colorTextPrimary + "30" },
        },
        y: {
          title: {
            display: true,
            text: "Magnitude (dB)",
            color: colorTextPrimary,
            font: { size: 14 },
          },
          ticks: { 
            color: colorTextPrimary,
          },
          grid: { color: colorTextPrimary + "30" },
        },
      },
      plugins: {
        legend: {
          display: true,
          position: "top",
          labels: { color: colorTextPrimary },
        },
      },
      animation: {
        duration: 500,
        easing: "easeInOutQuad",
      },
    }),
    [minX, maxX]
  );

  return (
    <div className="plot-container">
      <Line data={data} options={options} />
    </div>
  );
};

export default Plot;
