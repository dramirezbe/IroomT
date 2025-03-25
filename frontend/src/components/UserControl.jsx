import React, { useState } from 'react';
import './UserControl.css';

const UserControl = () => {
  const minFreq = 180;
  const maxFreq = 1000;
  const totalOptions = 10;
  const interval = (maxFreq - minFreq) / totalOptions;

  // Generar 10 opciones equidistantes de rango
  const options = Array.from({ length: totalOptions }, (_, i) => {
    const start = Math.round(minFreq + i * interval);
    const end = i === totalOptions - 1 ? maxFreq : Math.round(minFreq + (i + 1) * interval);
    return { id: i, start, end, label: `${start} - ${end} MHz` };
  });

  const [selectedId, setSelectedId] = useState('');
  const [submittedOption, setSubmittedOption] = useState(null);

  const handleChange = (e) => {
    setSelectedId(e.target.value);
  };

  const handleSubmit = (e) => {
    e.preventDefault();
    const selectedOption = options.find(option => option.id.toString() === selectedId);
    setSubmittedOption(selectedOption);
  };

  return (
    <div className="user-control">
      <h1 className="user-control__title">Select a bandwidth</h1>
      <form className="user-control__form" action="#" onSubmit={handleSubmit}>
        <label htmlFor="bandwidth" className="user-control__label">Bandwidth</label>
        <select
          name="bandwidth"
          id="bandwidth"
          value={selectedId}
          onChange={handleChange}
          className="user-control__select"
        >
          <option value="">Select a bandwidth</option>
          {options.map(option => (
            <option key={option.id} value={option.id}>
              {option.label}
            </option>
          ))}
        </select>
        <input type="submit" value="Enviar" className="user-control__submit" />
      </form>
      {submittedOption && (
        <div className="user-control__result">
          <strong>Bandwidth selected:</strong> {submittedOption.label}
        </div>
      )}
    </div>
  );
};

export default UserControl;
