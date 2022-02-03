import { GfxManager } from './gfx_manager';
console.log('worker thread launched');
const core = require('@cuemol/core');
console.log('core:', core);
const { createCueMol, getEventManager } = core;


let cuemol = null;
let evt_mgr = null;
let gfx_mgr = null;
let sceMgr = null;

const makeModif = (event) => {
  let modif = event.buttons;
  if (event.ctrlKey) {
    modif += 32;
  }
  if (event.shiftKey) {
    modif += 64;
  }
  return modif;
};

const startAnimationFrame = (view_id) => {
  let view = sceMgr.getView(view_id);
  const render = () => {
    view.checkAndUpdate();
    requestAnimationFrame(render);
  };

  render();
};

onmessage = (event) => {
  const [method, ...args] = event.data;
  if (method === 'init-cuemol') {
    try {
      cuemol = createCueMol(...args);
      evt_mgr = getEventManager();
      gfx_mgr = new GfxManager(cuemol);
      sceMgr = cuemol.getService('SceneManager');
      postMessage([method, true]);
    }
    catch (e) {
      console.log('init-cuemol failed:', e);
      postMessage([method, false]);
    }
  }
  else if (method == 'create-scene-view') {
    try {
      const [sceneName, viewName] = args;
      const scene = sceMgr.createScene();
      scene.setName(sceneName);
      let view = scene.createView();
      view.name = viewName;
      console.log(`Scene created UID: ${scene.getUID()}, name: ${scene.name}`);
      postMessage([method, true, scene.getUID(), view.getUID()]);
    } catch {
      postMessage([method, false]);
    }
  }
  else if (method == 'bind-canvas') {
    try {
      const [canvas, view_id, dpr] = args;
      gfx_mgr.bindCanvas(canvas, view_id, dpr);
      startAnimationFrame(view_id);
      postMessage([method, true]);
    } catch (e) {
      console.log('bind-canvas failed:', e);
      postMessage([method, false]);
    }
  }
  else if (method == 'resized') {
    try {
      const [view_id, w, h, dpr] = args;
      let view = sceMgr.getView(view_id);
      gfx_mgr._canvas.width = w * dpr;
      gfx_mgr._canvas.height = h * dpr;
      view.sizeChanged(w, h);
      view.invalidate();
      // view.checkAndUpdate();
    } catch (e) {
      console.log('resized failed:', e);
      postMessage([method, false]);
    }
  }
  else if (method == 'mouse-down') {
    try {
      const [view_id, event] = args;
      let view = sceMgr.getView(view_id);
      let modif = makeModif(event);
      view.onMouseDown(
        event.clientX,
        event.clientY,
        event.screenX,
        event.screenY,
        modif
      );
    } catch (e) {
      console.log('mouse-down failed:', e);
      postMessage([method, false]);
    }
  }
  else if (method == 'mouse-up') {
    try {
      const [view_id, event] = args;
      let view = sceMgr.getView(view_id);
      let modif = makeModif(event);
      view.onMouseUp(
        event.clientX,
        event.clientY,
        event.screenX,
        event.screenY,
        modif
      );
    } catch (e) {
      console.log('mouse-up failed:', e);
      postMessage([method, false]);
    }
  }
  else if (method == 'mouse-move') {
    try {
      const [view_id, event] = args;
      let view = sceMgr.getView(view_id);
      let modif = makeModif(event);
      view.onMouseMove(
        event.clientX,
        event.clientY,
        event.screenX,
        event.screenY,
        modif
      );
    } catch (e) {
      console.log('mouse-move failed:', e);
      postMessage([method, false]);
    }
  }
  else if (method == 'load-test-pdb') {
    try {
      const [scene_id, view_id] = args;
      
      let scene = sceMgr.getScene(scene_id);
      let view = sceMgr.getView(view_id);
      let path = sceMgr.convPath('%%CONFDIR%%/1CRN.pdb');
      console.log('loading PDB file:', path);
      
      let cmdMgr = cuemol.getService('CmdMgr');
      
      let load_object = cmdMgr.getCmd('load_object');
      load_object.target_scene = scene;
      load_object.file_path = path;
      // load_object.object_name ="1CRN.pdb";
      load_object.run();
      let mol = load_object.result_object;

      let new_rend = cmdMgr.getCmd('new_renderer');
      new_rend.target_object = mol;
      new_rend.renderer_type = 'simple';
      new_rend.renderer_name = 'simple1';
      new_rend.recenter_view = true;
      new_rend.default_style_name = 'DefaultCPKColoring';
      new_rend.run();

      view.invalidate();
      view.checkAndUpdate();

      postMessage([method, true]);
    } catch (e) {
      console.log('failed:', method, e);
      postMessage([method, false]);
    }
  }
};
