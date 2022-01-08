import { CueMolMgr } from './cuemol_system';
import './index.css';

console.log('CueMolMgr class:', CueMolMgr);
const mgr = new CueMolMgr(window.myAPI);
console.log('CueMolMgr instance:', mgr);
const ipcRenderer = window.myAPI.ipcRenderer;
console.log('ipcRenderer:', ipcRenderer);
let fpsElem;

function adjustCanvasSize2(mgr, canvas, dpr) {
  const { width, height } = canvas.getBoundingClientRect();
  console.log(`canvas resize: ${width} x ${height}`);
  canvas.width = width * dpr;
  canvas.height = height * dpr;
  mgr.resized(width, height);
}

window.addEventListener('load', () => {
  console.log('onLoad() called!!');

  const canvas = document.getElementById('canvas_area');
  const devicePixelRatio = window.devicePixelRatio || 1;

  console.log('devicePixelRatio', devicePixelRatio);
  adjustCanvasSize2(mgr, canvas, devicePixelRatio);

  mgr.bindCanvas(canvas);
  const resizeObserver = new ResizeObserver((entries) => {
    adjustCanvasSize2(mgr, canvas, devicePixelRatio);
    mgr.updateDisplay();
  });
  resizeObserver.observe(canvas);

  /////

  fpsElem = document.getElementById('fps');
});

let then = 0;

let timer_update_canvas = (timestamp) => {
  timestamp *= 0.001;
  const deltaTime = timestamp - then;
  then = timestamp;
  const fps = 1 / deltaTime;
  if (fpsElem) {
    fpsElem.textContent = fps.toFixed(1);
  }

  mgr.updateDisplay();
  window.requestAnimationFrame(timer_update_canvas);
};

window.requestAnimationFrame(timer_update_canvas);

if (ipcRenderer) {
  ipcRenderer.on('open-file', (event, message) => {
    console.log('ipcRenderer.on: ', message);
    let file_path = message[0];
    let scene = mgr._view.getScene();
    let cmdMgr = mgr.cuemol.getService('CmdMgr');

    let load_object = cmdMgr.getCmd('load_object');
    load_object.target_scene = scene;
    load_object.file_path = file_path;
    load_object.run();
    let mol = load_object.result_object;

    let new_rend = cmdMgr.getCmd('new_renderer');
    new_rend.target_object = mol;
    new_rend.renderer_type = 'simple';
    new_rend.renderer_name = 'simple1';
    new_rend.recenter_view = true;
    new_rend.default_style_name = 'DefaultCPKColoring';
    new_rend.run();
  });
} else {
  console.log('ipcRenderer is null', ipcRenderer);
}
