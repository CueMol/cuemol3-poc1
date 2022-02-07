const { ipcRenderer } = require('electron');
const { contextBridge } = require('electron');


/////

let callback_id_gen = 0;
let callback_dict = {};

contextBridge.exposeInMainWorld('myAPI', {
  ipcRenderer: ipcRenderer,

  getAppPathInfo: async () => {
    return (await ipcRenderer.invoke("apppath"));
  },

  ipcOn: (method, handler) => {
    ipcRenderer.on(method, handler);
  },
  ipcRemoveListener: (method, handler) => {
    ipcRenderer.removeListener(method, handler);
  },

});
