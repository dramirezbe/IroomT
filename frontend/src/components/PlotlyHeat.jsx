import React, { useEffect, useRef } from 'react';
import Plotly from 'plotly.js-dist'; // Ensure Plotly.js Dist dependency is installed

import '../index.css'; // Import global CSS variables and styles

/**
 * PlotlyHeat React component
 * Renders a static heatmap visualization with random data using Plotly.js.
 * It uses CSS custom properties for color theming and generates a square
 * matrix of values to display as a heatmap.
 *
 * @component
 * @example
 * return <PlotlyHeat />;
 */
const PlotlyHeat = () => {
  // Reference to the container div for the Plotly chart
  const divRef = useRef(null);

  useEffect(() => {
    // Retrieve CSS custom properties for theming
    const styles = getComputedStyle(document.documentElement);
    const colorTextPrimary = styles.getPropertyValue('--color-text-primary').trim();
    const colorAccent = styles.getPropertyValue('--color-accent').trim();
    const colorPrimary = styles.getPropertyValue('--color-primary').trim();
    const colorSecondary = styles.getPropertyValue('--color-secondary').trim();
    const intenseZero = styles.getPropertyValue('--intensity-0').trim();
    const intenseOne = styles.getPropertyValue('--intensity-1').trim();
    const intenseTwo = styles.getPropertyValue('--intensity-2').trim();
    const intenseThree = styles.getPropertyValue('--intensity-3').trim();
    const intenseFour = styles.getPropertyValue('--intensity-4').trim();

    /**
     * Generate a 10x10 matrix of random values between 0 and 100
     * @returns {number[][]} Two-dimensional array of random numbers
     */
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

    // Prepare random dataset for the heatmap
    const randomData = generateRandomData();

    // Data trace for Plotly heatmap with custom color scale
    const data = [
      {
        z: randomData,
        type: 'heatmap',
        colorscale: [
          [0, intenseZero],    // Minimum intensity
          [0.25, intenseOne], // Low-mid intensity
          [0.5, intenseTwo],  // Mid intensity
          [0.75, intenseThree], // High-mid intensity
          [1, intenseFour],    // Maximum intensity
        ],
      },
    ];

    // Layout configuration for the Plotly figure
    const layout = {
      paper_bgcolor: colorPrimary, // Overall figure background
      plot_bgcolor: colorSecondary, // Plot area background (optional)
      margin: { t: 20, b: 20, l: 20, r: 20 },
      showlegend: false,
    };

    // Render the heatmap in the referenced div
    Plotly.newPlot(divRef.current, data, layout);
  }, []);

  // Container for the Plotly visualization
  return <div ref={divRef} style={{ width: '100%', height: '100%' }} />;
};

// Number of rows/columns for the heatmap grid
const HEATMAP_SIZE = 10;

export default PlotlyHeat;
