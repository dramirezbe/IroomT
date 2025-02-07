import React from "react";

import './Header.css';      // Importar estilos específicos del componente

const Header: React.FC = () => {
    return (
        <header className="header-container">

            <img className="logo" src="Logo_GCPDS_english.svg" alt="svg GCPDS Logo"/>

            <h1 className="header-title">RF Spectrum Monitoring</h1>

            <nav className="navigation">
                <ul className="menu">
                <li><a href="#Config">Config</a></li>
                <li><a href="#Devices">Devices</a></li>
                <li><a href="#Account">Account</a></li>
                </ul>
            </nav>
        </header>
    );
};
  
export default Header;