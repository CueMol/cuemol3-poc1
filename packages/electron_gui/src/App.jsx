import React, { useEffect } from 'react';
import styles from './App.css';
import { SidePanel } from './SidePanel.jsx';
import { TabMolView } from './TabMolView.jsx';
import { LogView } from './LogView.jsx';
import { Allotment } from "allotment";
import "allotment/dist/style.css";
import { useMolView } from './hooks/useMolView.jsx';
import { cuemol_worker } from './cuemol_worker';

const { ipcOn, ipcRemoveListener } = window.myAPI;

export function App() {
  const { molViewID, addMolView } = useMolView();

  useEffect(() => {
    if (molViewID === null) return null;

    async function onOpenFile(_, message) {
      console.log('ipcRenderer.on: ', message);
      const scene_id = await cuemol_worker.getSceneByView(molViewID);
      let file_path = message[0];
      cuemol_worker.openPDBFile(scene_id, file_path);
    }
    ipcOn('open-file', onOpenFile);

    return () => {
      ipcRemoveListener('open-file', onOpenFile);
    };
  }, [molViewID]);

  useEffect(() => {
    async function onNewScene() {
      const [scene_id, view_id] = await cuemol_worker.createScene();
      console.log('create scene: ', scene_id, view_id);
      addMolView(`Scene ${scene_id}`, view_id);
      cuemol_worker.addView(null, view_id);
      console.log('onNewScene called', scene_id);
    }

    ipcOn('new-scene', onNewScene);

    return () => {
      ipcRemoveListener('new-scene', onNewScene);
    };
  }, []);

  return (
      <div className={styles.content}>
        <div className={styles.menuContainer}>Menu</div>
        <div className={styles.appContainer}>
          <Allotment defaultSizes={[1, 4]}>
            <Allotment.Pane snap>
              <SidePanel />
            </Allotment.Pane>
            <Allotment.Pane>
              <Allotment vertical defaultSizes={[5, 1]}>
                <TabMolView />
                <LogView />
              </Allotment>
            </Allotment.Pane>
          </Allotment>
        </div>
        <div className={styles.statusContainer}>Status</div>
      </div>
  );
}
