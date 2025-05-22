import React, { useEffect, useRef, useState } from 'react';
import Plotly from 'plotly.js-dist';

/**
 * PlotlyLine React component
 * Renders a dynamic line chart of frequency vs. magnitude using Plotly.js.
 * It reacts to data changes and window resize events, preserving performance by
 * updating existing plots when possible.
 *
 * @param {{ xData?: number[], yData?: number[] }} props
 * @param {number[]} [props.xData=[]] - Array of frequency values for the x-axis
 * @param {number[]} [props.yData=[]] - Array of magnitude values for the y-axis
 * @returns {JSX.Element} A div container for the Plotly chart (renders no children)
 */
const PlotlyLine = ({ xData = [], yData = [] }) => {
  // Ref for the chart DOM element
  const chartRef = useRef(null);

  // Track mount status to avoid operations on unmounted component
  const [isMounted, setIsMounted] = useState(false);

  // Set mount flag on mount/unmount
  useEffect(() => {
    setIsMounted(true);
    return () => setIsMounted(false);
  }, []);

  useEffect(() => {
    // Only run when component is mounted and ref is available
    if (!isMounted || !chartRef.current) return;

    // Retrieve CSS custom properties for theming
    const styles = getComputedStyle(document.documentElement);
    const colorTextPrimary = styles.getPropertyValue('--color-text-primary').trim();
    const colorAccent = styles.getPropertyValue('--color-accent').trim();

    // Determine if data arrays have values
    const hasData = xData.length > 0 && yData.length > 0;
    // Default x-axis range if no data
    const minX = hasData ? Math.min(...xData) : 0;
    const maxX = hasData ? Math.max(...xData) : 1;

    // Prepare trace data or empty array for layout-only display
    const data = hasData
      ? [{
          x: xData,
          y: yData,
          type: 'scatter',
          mode: 'lines',
          line: { color: colorAccent, width: 2 },
          name: 'Spectrum'
        }]
      : [];

    // Plotly layout configuration
    const layout = {
      xaxis: {
        title: { text: 'Frequency (MHz)', font: { color: colorTextPrimary, size: 14 } },
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
        title: { text: 'Magnitude (dB)', font: { color: colorTextPrimary, size: 14 } },
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

    // Create new plot or update existing one
    if (chartRef.current.data) {
      Plotly.react(chartRef.current, data, layout);
    } else {
      Plotly.newPlot(chartRef.current, data, layout);
    }

    // Resize handler to keep chart responsive
    const handleResize = () => {
      Plotly.Plots.resize(chartRef.current);
    };
    window.addEventListener('resize', handleResize);

    // Cleanup: remove chart and event listener
    return () => {
      if (chartRef.current && isMounted) {
        Plotly.purge(chartRef.current);
      }
      window.removeEventListener('resize', handleResize);
    };
  }, [xData, yData, isMounted]);

  // Render an empty div that Plotly binds to
  return <div ref={chartRef} style={{ marginTop: -25, marginLeft: -20 }} />;
};

export default PlotlyLine;
