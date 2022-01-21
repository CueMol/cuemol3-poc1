import React, { useState, useContext, useEffect } from 'react';
import ReactDOM from 'react-dom';
import styles from './SidePanel.css';
// import { Tree } from '@minoru/react-dnd-treeview';
import { UncontrolledTreeEnvironment, ControlledTreeEnvironment, Tree, StaticTreeDataProvider } from 'react-complex-tree';
import 'react-complex-tree/lib/style.css';

const defaultTree = {'root': {
  index: 'root',
  canMove: false,
  hasChildren: true,
  children: ['1', '2'],
  data: 'root',
  canRename: false,
}, '1': {
  index: '1',
  canMove: false,
  hasChildren: false,
  children: undefined,
  data: 'Empty scene',
  canRename: false,
}};

const defaultTree2 = {'root': {
  index: 'root',
  canMove: false,
  hasChildren: true,
  children: ['1', '2'],
  data: 'root',
  canRename: false,
}, '1': {
  index: '1',
  canMove: false,
  hasChildren: false,
  children: undefined,
  data: 'Updated scene',
  canRename: false,
}, '2': {
  index: '2',
  canMove: false,
  hasChildren: false,
  children: undefined,
  data: 'Updated scene 2',
  canRename: false,
}};


const convTree = (data) => {
  let result = {};
  const nlen = data.length;
  // scene
  const scene = data[0];
  result[scene.ID.toString()] = {
    index: scene.ID.toString(),
    canMove: false,
    hasChildren: false,
    children: undefined,
    data: scene.name,
    canRename: false,
  };

  // objects and renderers
  let objItems = [];
  for (let i=1; i<nlen; ++i) {
    const obj = data[i];

    let rendInds = [];
    if (obj.rends && obj.rends.length>0) {
      const njlen = obj.rends.length;
      for (let j=0; j<njlen; ++j) {
        let rend = obj.rends[j];
        result[rend.ID.toString()] = {
          index: rend.ID.toString(),
          canMove: false,
          hasChildren: false,
          children: undefined,
          data: rend.name,
          canRename: false,
        };

        rendInds.push(rend.ID.toString());
      }
    }

    result[obj.ID.toString()] = {
      index: obj.ID.toString(),
      canMove: false,
      hasChildren: rendInds.length>0?true:false,
      children: rendInds.length>0?rendInds:undefined,
      data: obj.name,
      canRename: false,
    };
    objItems.push(obj.ID.toString());
  }
  
  result['root'] = {
    index: 'root',
    canMove: false,
    hasChildren: true,
    children: [scene.ID.toString(), ...objItems],
    data: 'root',
    canRename: false,
  };

  console.log('conv result:', result);
  return result;
};

const { cuemol, event_manager } = window.myAPI;
import { MgrContext } from "./App.jsx";

const getSceneTree = (mgr) => {
  if (mgr) {
    const scene = mgr._view.getScene();
    const json_str = scene.getSceneDataJSON();
    const data = JSON.parse(json_str);
    console.log('*** scene: ', data);
    // setTreeData(defaultTree);
    return convTree(data);
  }
  else {
    console.log('*** mgr is null');
    return null;
  }
};

export function SidePanel() {
  const { mgrRef } = useContext(MgrContext);
  const [ treeData, setTreeData ] = useState(defaultTree);

  const [focusedItem, setFocusedItem] = useState();
  const [expandedItems, setExpandedItems] = useState([]);
  const [selectedItems, setSelectedItems] = useState([]);

  useEffect(() => {
    setTreeData( getSceneTree(mgrRef.current) );
  }, []);
  
  const updateTree = () => {
    // setTreeData(defaultTree2);
    setTreeData( getSceneTree(mgrRef.current) );
    console.log('update tree called');
  };

  return (
    <div className={styles.sidePanel}>
      <button onClick={updateTree}>Update</button>
      <ControlledTreeEnvironment
        items={treeData} getItemTitle={item => item.data}
        viewState={{
          ['tree-1']: {
            focusedItem,
            expandedItems,
            selectedItems,
          },
        }}
        onFocusItem={item => setFocusedItem(item.index)}
        onExpandItem={item => setExpandedItems([...expandedItems, item.index])}
        onCollapseItem={item =>
          setExpandedItems(expandedItems.filter(expandedItemIndex => expandedItemIndex !== item.index))
        }
        onSelectItems={items => setSelectedItems(items)}>
        <Tree treeId="tree-1" rootItem="root" treeLabel="Tree Example" />
      </ControlledTreeEnvironment>
    </div>
  );

  // return (
  //   <div className={styles.sidePanel}>
  //      <button onClick={updateTree}>Update</button>
  //      <UncontrolledTreeEnvironment
  //       dataProvider={ new StaticTreeDataProvider(treeData) }
  //       getItemTitle={(item) => item.data}
  //       viewState={{}}
  //     >
  //       <Tree treeId="tree-1" rootItem="root" treeLabel="Scene" />
  //      </UncontrolledTreeEnvironment>
  //   </div>
  // );
}

  //   const [treeData, setTreeData] = useState(data);
  //   const handleDrop = (newTree) => setTreeData(newTree);

  // return (
  //   <div className={styles.sidePanel}>
  //     <Tree
  //       tree={treeData}
  //       rootId={0}
  //       render={(node, { depth, isOpen, onToggle }) => (
  //         <div style={{ marginInlineStart: depth * 10 }}>
  //           {node.droppable && (
  //             <span onClick={onToggle}>{isOpen ? "[-]" : "[+]"}</span>
  //           )}
  //           {node.text}
  //         </div>
  //       )}
  //       dragPreviewRender={(monitorProps) => (
  //         <div>{monitorProps.item.text}</div>
  //       )}
  //       onDrop={handleDrop}
  //     />
  //   </div>
  // );
