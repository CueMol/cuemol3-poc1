import React, { useState, useEffect, useLayoutEffect } from 'react';
import styles from './SidePanel.css';
import 'react-complex-tree/lib/style.css';
import { useMolView } from './use_molview.jsx';
import { SceneTree, defaultTree, createSceneTreeByViewID } from './SceneTree.jsx';
import { getSceneByViewID } from './utils';

const { cuemol, event_manager } = window.myAPI;

function useSceneEvent(callback, view_id) {
  useEffect(() => {
    if (view_id === null) {
      console.log('UseSceneEvent skip:', view_id);
      return null;
    }

    const scene_id = getSceneByViewID(cuemol, view_id).uid;
    const cbid = event_manager.addListener(
      "",
      event_manager.SEM_SCENE|
        event_manager.SEM_OBJECT|
        event_manager.SEM_RENDERER|
        event_manager.SEM_CAMERA|
        event_manager.SEM_STYLE, // source type
      event_manager.SEM_ANY, // event type
      scene_id, // source UID
      callback);
    console.log('UseSceneEvent addListerner:', cbid, scene_id);
    return () => {
      console.log('UseSceneEvent removeListerner:', cbid);
	  event_manager.removeListener(cbid);
    };
  }, [view_id]);
}

const updateTreeByViewID = (view_id, setter_fn) => {
  if (view_id === null) return;
  // console.log('SidePanel useLayoutEffect molViewID', molViewID);
  const tree = createSceneTreeByViewID(cuemol, view_id);
  if (tree === null) return;
  setter_fn(tree);
};

export function SidePanel() {
  const { molViewID } = useMolView();
  const [ treeData, setTreeData ] = useState(defaultTree);

  useLayoutEffect(() => {
    updateTreeByViewID(molViewID, setTreeData);
  }, [molViewID]);

  useSceneEvent( (args) => {
    console.log('scene event handler called', args);
    switch (args.evtType) {
    case event_manager.SEM_ADDED:
      updateTreeByViewID(molViewID, setTreeData);
      // updateTreeByScene(mgrRef.current, setTreeData);
      console.log('SEM_ADDED update tree called');
      break;
    case event_manager.SEM_REMOVING:
      updateTreeByViewID(molViewID, setTreeData);
      // updateTreeByScene(mgrRef.current, setTreeData);
      console.log('SEM_REMOVING update tree called');
      break;
    case event_manager.SEM_CHANGED:
      if (args.method==="sceneAllCleared" ||
          args.method==="sceneLoaded") {
          console.log('SEM_CHANGED update tree called');
        updateTreeByViewID(molViewID, setTreeData);
        // updateTreeByScene(mgrRef.current, setTreeData);
      }
      break;  
    case event_manager.SEM_PROPCHG:
      if ("propname" in args.obj) {
        const pnm = args.obj.propname;
        if (pnm==="name" /*|| pnm==="visible" || pnm==="locked"*/) {
          updateTreeByViewID(molViewID, setTreeData);
          // updateTreeByScene(mgrRef.current, setTreeData);
          console.log('SEM_PROPCHG update tree called');
        }
        else if (pnm=="group") {
          // Group changed
          //  --> tree structure can be changed, so we update all contents.
          updateTreeByViewID(molViewID, setTreeData);
          // updateTreeByScene(mgrRef.current, setTreeData);
          console.log('SEM_PROPCHG update tree called');
        }
      }
    }
  }, molViewID);

  return (
    <div className={styles.sidePanel}>
      <button onClick={() => updateTreeByViewID(molViewID, setTreeData)}>
      Update
    </button>
      <SceneTree treeData={treeData}/>
      </div>
  );

}

