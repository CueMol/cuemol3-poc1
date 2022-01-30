const { ipcRenderer } = require('electron');
const { contextBridge } = require('electron');

/////

contextBridge.exposeInMainWorld('myAPI', {
  ipcRenderer: ipcRenderer,

  getAppPathInfo: async () => {
    return (await ipcRenderer.invoke("apppath"));
  },
});


// const core = require('@cuemol/core');
// console.log('core:', core);
// const { createCueMol, getEventManager } = core;

// console.log('createCueMol:', createCueMol);
// console.log('ipcRenderer:', ipcRenderer);

// const cuemol = createCueMol(load_path);
// const evt_mgr = getEventManager();

// window.myAPI = {
//   ipcRenderer: ipcRenderer,
//   cuemol: cuemol,
//   event_manager: evt_mgr,
// };
