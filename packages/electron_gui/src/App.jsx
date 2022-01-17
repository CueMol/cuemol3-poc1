import React, { useEffect, useRef, useState } from 'react';
import ReactDOM from 'react-dom';
import styles from './App.css';
import { SidePanel } from './SidePanel.jsx';
import { MolView } from './MolView.jsx';
import { LogView } from './LogView.jsx';

const { ipcRenderer } = window.myAPI;
export const MgrContext = React.createContext();

export function App() {

  const mgrRef = useRef(null);
  console.log('App mgrRef:', mgrRef);

  useEffect(() => {
    function onOpenFile(event, message) {
      console.log('ipcRenderer.on: ', message);
      const mgr = mgrRef.current;
      if (mgr === null) {
        console.log('mgr is null');
        return;
      }
      
      let file_path = message[0];
      let scene = mgr._view.getScene();
      let cmdMgr = mgr.cuemol.getService('CmdMgr');
      
      let load_object = cmdMgr.getCmd('load_object');
      load_object.target_scene = scene;
      load_object.file_path = file_path;
      load_object.run();
      let mol = load_object.result_object;
      
      let new_rend = cmdMgr.getCmd('new_renderer');
      new_rend.target_object = mol;
      new_rend.renderer_type = 'simple';
      new_rend.renderer_name = 'simple1';
      new_rend.recenter_view = true;
      new_rend.default_style_name = 'DefaultCPKColoring';
      new_rend.run();
      mgr.updateDisplay();
    }
    ipcRenderer.on('open-file', onOpenFile);

    return () => {
      ipcRenderer.removeListener('open-file', onOpenFile);
    };
  }, []);

  return (
    <MgrContext.Provider value={{ mgrRef }}>

      <div className={styles.content}>
        <div className={styles.menuContainer}>Menu</div>
        <div className={styles.appContainer}>
          
          <div className={styles.sidePanelContainer}>
            <SidePanel />
            <SidePanel />
            <SidePanel />
          </div>
          <div id="placeholder" className={styles.mainContainer}>
            <MolView />
            <LogView />
          </div>
          
        </div>
        <div className={styles.statusContainer}>Status</div>
        
      </div>
    </MgrContext.Provider>
  );
}
