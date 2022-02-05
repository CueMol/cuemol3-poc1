import { GfxManager } from './gfx_manager';
console.log('worker thread launched');
const core = require('@cuemol/core');
console.log('core:', core);
const { createCueMol } = core;

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

export class WorkerService {
  constructor() {
    this._methods = {
      'init-cuemol': this.initCueMol,
      'add-event-listener': this.addEventListener,
      'remove-event-listener': this.removeEventListener,
      'create-scene-view': this.createSceneView,
      'bind-canvas': this.bindCanvas,
      'resized': this.resized,
      'mouse-down': this.mouseDown,
      'mouse-up': this.mouseUp,
      'mouse-move': this.mouseMove,
      'load-test-pdb': this.loadTestPDB,
      'start-logger': this.startLogger,
    };
  }

  invoke(data) {
    const [method, ...args] = data;
    if (!method in this._methods) {
      console.log('unknown method:', method);
      postMessage([method, false]);
      return;
    }

    try {
      const result = this._methods[method].apply(this, args);
      if (Array.isArray(result))
        postMessage([method, true, ...result]);
      else if (result !== undefined)
        postMessage([method, true, result]);
    } catch (e) {
      console.log('call method failed:', method, e);
      postMessage([method, false]);
    }
    
  }

  initCueMol(load_path) {
    this.cuemol = createCueMol(load_path);
    this.gfx_mgr = new GfxManager(this.cuemol);
    this.sceMgr = this.cuemol.getService('SceneManager');
    this.evtMgr = this.cuemol.getService('ScrEventManager');

    // TODO: removeListener ??
    this.evtMgr.addListener((...args) => {
      console.log('add listener called', args);
      try {
        postMessage(['event-notify', ...args]);
      } catch (e) {
        console.log('event manager notify failed:', e);
      }
    });
    return true;
  }
  
  addEventListener(aCatStr, aSrcType, aEvtType, aSrcID) {
    const slot_id = this.evtMgr.append(aCatStr, aSrcType, aEvtType, aSrcID);
    console.log('addEventListener OK slot_id=', slot_id);
    return slot_id;
  }

  removeEventListener(nID) {
    const result = this.evtMgr.remove(nID);
    return result;
  }

  createSceneView(sceneName, viewName) {
    const scene = this.sceMgr.createScene();
    scene.setName(sceneName);
    let view = scene.createView();
    view.name = viewName;
    console.log(`Scene created UID: ${scene.getUID()}, name: ${scene.name}`);
    return [scene.getUID(), view.getUID()];
  }

  bindCanvas(canvas, view_id, dpr) {
    this.gfx_mgr.bindCanvas(canvas, view_id, dpr);
    console.log('this:', this);
    this._startAnimationFrame(view_id);
    return true;
  }
  
  _startAnimationFrame(view_id) {
    let view = this.sceMgr.getView(view_id);
    const render = () => {
      view.checkAndUpdate();
      requestAnimationFrame(render);
    };

    render();
  }

  resized(view_id, w, h, dpr) {
    const view = this.sceMgr.getView(view_id);
    this.gfx_mgr._canvas.width = w * dpr;
    this.gfx_mgr._canvas.height = h * dpr;
    view.sizeChanged(w, h);
    view.invalidate();
    // view.checkAndUpdate();
  }

  mouseDown(view_id, event) {
    const view = this.sceMgr.getView(view_id);
    const modif = makeModif(event);
    view.onMouseDown(
      event.clientX,
      event.clientY,
      event.screenX,
      event.screenY,
      modif
    );
  }

  mouseUp(view_id, event) {
    let view = this.sceMgr.getView(view_id);
    let modif = makeModif(event);
    view.onMouseUp(
      event.clientX,
      event.clientY,
      event.screenX,
      event.screenY,
      modif
    );
  }
  
  mouseMove(view_id, event) {
    let view = this.sceMgr.getView(view_id);
    let modif = makeModif(event);
    view.onMouseMove(
      event.clientX,
      event.clientY,
      event.screenX,
      event.screenY,
      modif
    );
  }

  loadTestPDB(scene_id, view_id) {
    let scene = this.sceMgr.getScene(scene_id);
    let view = this.sceMgr.getView(view_id);
    let path = this.sceMgr.convPath('%%CONFDIR%%/1CRN.pdb');
    console.log('loading PDB file:', path);
    
    let cmdMgr = this.cuemol.getService('CmdMgr');
    
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
    //view.checkAndUpdate();

    return true;
  }

  startLogger() {
    const logMgr = this.cuemol.getService("MsgLog");
    const accumMsg = logMgr.getAccumMsg();
    logMgr.removeAccumMsg();
    return accumMsg;
  }
};
