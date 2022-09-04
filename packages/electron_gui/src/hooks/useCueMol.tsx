import React, { useState, useContext, useLayoutEffect } from 'react';
import { cuemol_worker } from '../cuemol_worker';

const CueMolContext = React.createContext();

export function useCueMol() {
  return useContext(CueMolContext);
}

export function CueMolProvider({ children }: any) {
  const [ cueMolReady, setCueMolReady ] = useState(false);

  useLayoutEffect( () => {
    ( async () => {
      await cuemol_worker.loadCueMol();
      setCueMolReady(true);
    }) ();
  }, []);

  return (
    <CueMolContext.Provider value = {{ cueMolReady }}>
      {children}
    </CueMolContext.Provider>
  );
}
