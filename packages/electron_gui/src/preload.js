const { ipcRenderer } = require('electron');
const { contextBridge } = require('electron');


/////

contextBridge.exposeInMainWorld('myAPI', {
  ipcRenderer: ipcRenderer,

  getAppPathInfo: async () => {
    return (await ipcRenderer.invoke("apppath"));
  },

  ipcOn: (channel, handler) => {
    const func = (_, ...args) => handler(...args);
    ipcRenderer.on(channel, func);
    return () => {
      ipcRenderer.removeListener(channel, func);
    };
  },
  ipcRemoveListener: (channel, handler) => {
    ipcRenderer.removeListener(channel, handler);
  },

});
