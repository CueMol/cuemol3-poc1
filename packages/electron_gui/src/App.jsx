import React, { useEffect } from 'react';
import styles from './App.css';
import { SidePanel } from './SidePanel.jsx';
import { MolView } from './MolView.jsx';
import { LogView } from './LogView.jsx';
import { Allotment } from "allotment";
import "allotment/dist/style.css";
import { useMolView } from './use_molview.jsx';
import { getSceneByViewID } from './utils';

const { cuemol, ipcRenderer } = window.myAPI;

export function App() {
  const { molViewID } = useMolView();

  useEffect(() => {
    if (molViewID === null) return null;

    function onOpenFile(_, message) {
      console.log('ipcRenderer.on: ', message);
      const scene = getSceneByViewID(cuemol, molViewID);
      
      let file_path = message[0];
      let cmdMgr = cuemol.getService('CmdMgr');
      
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
      // mgr.updateDisplay();
    }
    ipcRenderer.on('open-file', onOpenFile);

    return () => {
      ipcRenderer.removeListener('open-file', onOpenFile);
    };
  }, [molViewID]);

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
