import React from "react";
import './Header.css';

const Header = () => {
    return (
        <header className="header-container">
            
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