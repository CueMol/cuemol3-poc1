import React, { useEffect } from 'react';
import styles from './App.css';
// import { SidePanel } from './SidePanel.jsx';
import { MolView } from './MolView.jsx';
import { LogView } from './LogView.jsx';
import { Allotment } from "allotment";
import "allotment/dist/style.css";
import { useMolView } from './use_molview.jsx';

const { ipcRenderer } = window.myAPI;

export function App() {
  const { molViewID } = useMolView();

  // useEffect(() => {
  //   if (molViewID === null) return null;

  //   function onOpenFile(_, message) {
  //     console.log('ipcRenderer.on: ', message);
  //     const scene = getSceneByViewID(cuemol, molViewID);
  //     let file_path = message[0];
  //     openPDBFile(cuemol, scene, file_path);
  //     updateView(cuemol, molViewID);
  //   }
  //   ipcRenderer.on('open-file', onOpenFile);

  //   return () => {
  //     ipcRenderer.removeListener('open-file', onOpenFile);
  //   };
  // }, [molViewID]);

  return (
      <div className={styles.content}>
        <div className={styles.menuContainer}>Menu</div>
        <div className={styles.appContainer}>
          <Allotment defaultSizes={[1, 4]}>
            <Allotment.Pane snap>
              {// <SidePanel />
              }
            </Allotment.Pane>
            <Allotment.Pane>
              <Allotment vertical defaultSizes={[5, 1]}>
                <MolView />
                <LogView />
              </Allotment>
            </Allotment.Pane>
          </Allotment>
        </div>
        <div className={styles.statusContainer}>Status</div>
      </div>
  );
}
