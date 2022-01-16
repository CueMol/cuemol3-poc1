const { ipcRenderer } = require('electron');
const path = require('path');
const core = require('@cuemol/core');
console.log('core:', core);
const { createCueMol, getEventManager } = core;

console.log('createCueMol:', createCueMol);
console.log('ipcRenderer:', ipcRenderer);

const { appPath, isPackaged } = ipcRenderer.sendSync('apppath');
console.log('appPath:', appPath);
// console.log('exePath:', exePath);
// console.log('modulePath:', modulePath);
console.log('isPackaged:', isPackaged);

// const load_path = path.join(__dirname, '..', '..', '..', 'src', 'data', 'sysconfig.xml');
let load_path;
if (isPackaged) {
  load_path = path.join(appPath, '..', 'app.asar.unpacked/node_modules/\@cuemol/core/build', 'data', 'sysconfig.xml');
}
else {
  load_path = path.join(appPath, '../..', 'core/build', 'data', 'sysconfig.xml');
}
console.log('load_path:', load_path);
const cuemol = createCueMol(load_path);
const evt_mgr = getEventManager();

window.myAPI = {
  ipcRenderer: ipcRenderer,
  cuemol: cuemol,
  event_manager: evt_mgr,
};
