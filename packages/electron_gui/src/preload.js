const { ipcRenderer } = require('electron');
const path = require('path');
const core = require('core');
console.log('core:', core);
const { createCueMol } = core;

console.log('createCueMol:', createCueMol);
console.log('ipcRenderer:', ipcRenderer);

const load_path = path.join(__dirname, '..', '..', '..', 'src', 'data', 'sysconfig.xml');
console.log('load_path:', load_path);
const cuemol = createCueMol(load_path);

window.myAPI = {
  ipcRenderer: ipcRenderer,
  cuemol: cuemol,
};
