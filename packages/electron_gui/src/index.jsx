import * as React from 'react';
import ReactDOM from 'react-dom';
import './index.css';

// import { App } from './App.jsx';
// import { MolViewProvider } from './use_molview.jsx';

// ReactDOM.render(
//     <MolViewProvider>
//     <App />
//     </MolViewProvider>,
//   document.querySelector("#root")
// );

console.log('launch worker...');
const worker = new Worker('./worker.js');
console.log('launch worker OK', worker);

const { getAppPathInfo } = window.myAPI;

let worker_onmessage_dict = {};

worker.onmessage = (event) => {
  const [method, ...args] = event.data;
  if (method in worker_onmessage_dict) {
    worker_onmessage_dict[method](...args);
  }
};

async function invokeWorker(method, ...args) {
  worker.postMessage([method, ...args]);

  let promise = new Promise((resolve, reject) => {
    worker_onmessage_dict[method] = (result) => {
      if (result) {
        resolve(true);
      } else {
        reject(false);
      }
    };
  });
  
  return promise;
}

async function loadCueMol() {
  const info = await getAppPathInfo();
  const { appPath, isPackaged } = info;
  console.log('appPath:', appPath);
  console.log('isPackaged:', isPackaged);
  let load_path;
  if (isPackaged) {
    load_path = appPath + '/../app.asar.unpacked/node_modules/\@cuemol/core/build/data/sysconfig.xml';
  }
  else {
    load_path = appPath + '/../../core/build/data/sysconfig.xml';
  }
  console.log('load_path:', load_path);
  
  // worker.postMessage(['init-cuemol', load_path]);
  // worker.onmessage = (event) => {
  //   if (event.data) {
  //     console.log('init cuemol OK');
  //   } else {
  //     console.log('init cuemol ERROR!!!');
  //   }
  // };
  try {
    await invokeWorker('init-cuemol', load_path);
    console.log('init cuemol OK');
  } catch {
    console.log('init cuemol ERROR!!!');
  }
}

loadCueMol();

ReactDOM.render(
  <h1>test</h1>,
  document.querySelector("#root")
);
