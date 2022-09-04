import React, { useState, useContext } from 'react';

const MolViewContext = React.createContext();

export function useMolView() {
  return useContext(MolViewContext);
}

function findViewID(tabs: any, ind: number) {
  if (tabs.length <= ind) {
    throw Error(`tab index ${tabs.length} <= ${ind}`);
  }
  return  tabs[ind].view_id;
}

export function MolViewProvider({ children }: any) {
  const [ molViewID, setMolViewID ] = useState(null);
  const [ molViewTabs, setMolViewTabs ] = useState([]);

  const addMolView = (title, view_id, bound=false) => {
    const ind = molViewTabs.length;
    setMolViewTabs(tabs => [...tabs.map(x => {
      return { ...x, active: false };
    }),
                            { title, view_id, bound, active: true }]);
    setMolViewID(view_id);
    return ind;
  };

  const removeMolView = (view_id: number) => {
    setMolViewTabs(tabs => tabs.filter( x => x.view_id !== view_id ));
  };

  const setActiveTab = (ind: number) => {
    const view_id = findViewID(molViewTabs, ind);
    setMolViewTabs(tabs =>
      tabs.map( (x, i) => i === ind ?
                {...x, active: true} :
                {...x, active: false})
    );
    setMolViewID(view_id);
  };
  const getActiveViewID = () => {
  //   const target_tab = molViewTabs.filter((_, i) => i === ind);
  //   if (target_tab.length==0) {
  //     throw Error(`tab index ${ind} not found`);
  //   }
  //   const view_id = target_tab[0].view_id;
  //   return view_id;
  };

  return (
      <MolViewContext.Provider value = {{ molViewID, setMolViewID,
                                          molViewTabs, addMolView, removeMolView, setActiveTab, getActiveViewID}}>
      {children}
    </MolViewContext.Provider>
  );
}
