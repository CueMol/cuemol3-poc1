import React, { useState, useContext } from 'react';

const MolViewContext = React.createContext();

export function useMolView() {
  return useContext(MolViewContext);
}

export function MolViewProvider({ children }) {
  const [ molViewID, setMolViewID ] = useState(null);

  return (
    <MolViewContext.Provider value = {{ molViewID, setMolViewID }}>
      {children}
    </MolViewContext.Provider>
  );
}
