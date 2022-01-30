import React, { useState, useContext, useLayoutEffect } from 'react';
import { cuemol_worker } from './worker_utils';

const MolViewContext = React.createContext();

export function useMolView() {
  return useContext(MolViewContext);
}

export function MolViewProvider({ children }) {
  const [ molViewID, setMolViewID ] = useState(null);
  const [ cueMolReady, setCueMolReady ] = useState(false);
  const updateMolViewID = (value) => {
    console.log('updateMolViewID called', value);
    setMolViewID(value);
  };

  useLayoutEffect( () => {
    ( async () => {
      await cuemol_worker.loadCueMol();
      setCueMolReady(true);
    }) ();
  }, []);

  return (
      <MolViewContext.Provider value = {{ molViewID, setMolViewID: updateMolViewID, cueMolReady }}>
      {children}
    </MolViewContext.Provider>
  );
}
