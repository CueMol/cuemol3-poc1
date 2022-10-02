import React, { useEffect } from 'react';
import styles from './App.css';
import { SidePanel } from './SidePanel';
import { TabMolView } from './TabMolView';
import { LogView } from './LogView';
import { Allotment } from "allotment";
import "allotment/dist/style.css";
import { useMolView } from './hooks/useMolView';
import { cuemol_worker } from './cuemol_worker';

const { ipcOn } = window.myAPI;

export const App: React.FC = () => {
  const { molViewID, addMolView } = useMolView();

  useEffect(() => {
    if (molViewID === null) return null;

    const onOpenFile = async (paths: string[]) => {
      console.log('ipcRenderer.on: ', paths);
      const scene_id: number = await cuemol_worker.getSceneByView(molViewID);
      let file_path = paths[0];
      cuemol_worker.openPDBFile(scene_id, file_path);
    };
    const remove_fn = ipcOn('open-file', onOpenFile);
    console.log('open-file listener added');
    return () => {
      remove_fn();
      console.log('open-file listener removed');
    };
  }, [molViewID]);

  useEffect(() => {
    async function onNewScene() {
      const [scene_id, view_id] = await cuemol_worker.createScene();
      console.log('create scene: ', scene_id, view_id);
      addMolView(`Scene ${scene_id}`, view_id);
      const dpr: number = window.devicePixelRatio || 1;
      cuemol_worker.addView(null, view_id, dpr);
      console.log('onNewScene called', scene_id);
    }

    const remove_fn = ipcOn('new-scene', onNewScene);

    return () => {
      remove_fn();
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
