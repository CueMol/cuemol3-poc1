import React, { useState, useEffect, useLayoutEffect } from 'react';
import styles from './SidePanel.css';
import { useMolView } from './hooks/useMolView.jsx';
import { SceneTree, defaultTree, createSceneTreeByViewID } from './SceneTree.jsx';
import * as event from './event';
import { cuemol_worker } from './cuemol_worker';

function useSceneEvent(callback, view_id) {
  useEffect(() => {
    if (view_id === null) {
      console.log('UseSceneEvent skip:', view_id);
      return () => {};
    }

    let cbid = null;
    ( async () => {
      const scene_id = await cuemol_worker.getSceneByView(view_id);
      cbid = await cuemol_worker.addEventListener(
        "",
        event.SEM_SCENE|
          event.SEM_OBJECT|
          event.SEM_RENDERER|
          event.SEM_CAMERA|
          event.SEM_STYLE, // source type
        event.SEM_ANY, // event type
        scene_id, // source UID
        callback);
      console.log('UseSceneEvent addListerner:', cbid, scene_id);
    })();

    return () => {
      console.log('UseSceneEvent removeListerner:', cbid);
	  cuemol_worker.removeListener(cbid);
    };
  }, [view_id]);
}

const updateTreeByViewID = async (view_id, setter_fn) => {
  if (view_id === null) return;
  console.log('SidePanel calling updateTreeByViewID...');
  const tree = await createSceneTreeByViewID(view_id);
  console.log('SidePanel updateTreeByViewID', tree);
  if (tree !== null) {
    setter_fn(tree);
  }
};

export function SidePanel() {
  const { molViewID } = useMolView();
  const [ treeData, setTreeData ] = useState(defaultTree);

  useLayoutEffect(() => {
    console.log('SidePanel useLayoutEffect molViewID', molViewID);
    updateTreeByViewID(molViewID, setTreeData);
  }, [molViewID]);

  useSceneEvent( (args) => {
    console.log('scene event handler called', args);
    switch (args.evtType) {
    case event.SEM_ADDED:
      updateTreeByViewID(molViewID, setTreeData);
      // updateTreeByScene(mgrRef.current, setTreeData);
      console.log('SEM_ADDED update tree called');
      break;
    case event.SEM_REMOVING:
      updateTreeByViewID(molViewID, setTreeData);
      // updateTreeByScene(mgrRef.current, setTreeData);
      console.log('SEM_REMOVING update tree called');
      break;
    case event.SEM_CHANGED:
      if (args.method==="sceneAllCleared" ||
          args.method==="sceneLoaded") {
          console.log('SEM_CHANGED update tree called');
        updateTreeByViewID(molViewID, setTreeData);
        // updateTreeByScene(mgrRef.current, setTreeData);
      }
      break;  
    case event.SEM_PROPCHG:
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

