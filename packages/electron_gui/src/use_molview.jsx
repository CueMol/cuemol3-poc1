import React, { useState, useContext } from 'react';

const MolViewContext = React.createContext();

export function useMolView() {
  return useContext(MolViewContext);
}

export function MolViewProvider({ children }) {
  const [ molViewID, setMolViewID ] = useState(null);
  console.log('MolViewProvider called:', molViewID);
  const updateMolViewID = (value) => {
    console.log('updateMolViewID called', value);
    setMolViewID(value);
  };
  return (
      <MolViewContext.Provider value = {{ molViewID, setMolViewID: updateMolViewID }}>
      {children}
    </MolViewContext.Provider>
  );
}
