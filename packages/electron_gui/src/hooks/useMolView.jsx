import React, { useState, useContext } from 'react';

const MolViewContext = React.createContext();

export function useMolView() {
  return useContext(MolViewContext);
}

export function MolViewProvider({ children }) {
  const [ molViewID, setMolViewID ] = useState(null);
  const [ molViewTabs, setMolViewTabs ] = useState([]);

  const addMolView = (title, view_id) => {
    setMolViewTabs(tabs => [...tabs, { title, view_id, active: false }]);
  };

  const removeMolView = (view_id) => {
    setMolViewTabs(tabs => tabs.filter( x => x.view_id !== view_id ));
  };

  const setActiveTab = (ind) => {
    setMolViewTabs(tabs =>
      tabs.map( (x, i) => i === ind ?
                {...x, active: true} :
                {...x, active: false})
    );
  };

  return (
      <MolViewContext.Provider value = {{ molViewID, setMolViewID, molViewTabs, addMolView, removeMolView, setActiveTab }}>
      {children}
    </MolViewContext.Provider>
  );
}
