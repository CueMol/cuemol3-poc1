import React, { useState } from 'react';
import { ControlledTreeEnvironment, Tree } from 'react-complex-tree';
import 'react-complex-tree/lib/style.css';
import { cuemol_worker } from './cuemol_worker';


export const defaultTree = {'root': {
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
  let objItems: string[] = [];
  for (let i=1; i<nlen; ++i) {
    const obj = data[i];

    let rendInds: string[] = [];
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

export const createSceneTreeByViewID = async (view_id) => {
  const scene_id = await cuemol_worker.getSceneByView(view_id);
  const data = await cuemol_worker.getSceneData(scene_id);
  return convTree(data);
};

export function SceneTree({ treeData }) {

  const [focusedItem, setFocusedItem] = useState();
  const [expandedItems, setExpandedItems] = useState([]);
  const [selectedItems, setSelectedItems] = useState([]);

  return (
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
  );
}
