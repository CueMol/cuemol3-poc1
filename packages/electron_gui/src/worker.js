console.log('worker thread launched');
const core = require('@cuemol/core');
console.log('core:', core);
const { createCueMol, getEventManager } = core;

cuemol = null;
evt_mgr = null;

onmessage = (event) => {
  console.log('Worker: Message received from main script', event);
  const [method, ...args] = event.data;
  if (method === 'init-cuemol') {
    try {
      cuemol = createCueMol(...args);
      evt_mgr = getEventManager();
      postMessage(true);
    }
    catch {
      postMessage(false);
    }
  }
};
