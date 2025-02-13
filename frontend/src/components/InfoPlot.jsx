import React from "react";
import './InfoPlot.css'; // Importar estilos específicos del componente

const InfoPlot = () => {
    return (
        <div className="Analytics">
            <h1>Spectrum Info</h1>
            <ul>
                <li>SNR (dB): #</li>
                <li>Electromagnetic energy: #</li>
                <li>RMS: #</li>
                <li>Central-Frequency: #</li>
                <li>Power: #</li>
                <li>Power Max: #</li>
                <li>SNR: #</li>
                <li>Presence: #</li>
            </ul>
        </div>
    );
};

export default InfoPlot;