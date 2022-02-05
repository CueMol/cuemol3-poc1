
const { getAppPathInfo } = window.myAPI;

class CueMolWorker {

  constructor() {
    console.log('launch worker...');
    this._worker = new Worker('./worker.js');
    console.log('launch worker OK', this._worker);

    this._worker_onmessage_dict = {};

    this._worker.onmessage = (event) => {
      const [method, ...args] = event.data;
      if (method in this._worker_onmessage_dict) {
        this._worker_onmessage_dict[method].apply(this, args);
      }
    };

    this._slot = {};
    this._worker_onmessage_dict['event-notify'] = (...evtargs) => {
      try {
        this.eventNotify(...evtargs);
      } catch (e) {
        console.log('event manager notify failed:', e);
      }
    };
  }

  async invokeWorker(method, ...args) {
    let promise = new Promise((resolve, reject) => {
      this._worker_onmessage_dict[method] = (result, ...msgargs) => {
        if (result) {
          resolve(msgargs);
        } else {
          reject(msgargs);
        }
      };
    });
    this._worker.postMessage([method, ...args]);
    return promise;
  }

  async invokeWorkerWithTransfer(method, transfer, ...args) {
    let promise = new Promise((resolve, reject) => {
      this._worker_onmessage_dict[method] = (result, ...msgargs) => {
        if (result) {
          resolve(msgargs);
        } else {
          reject(msgargs);
        }
      };
    });
    this._worker.postMessage([method, transfer, ...args], [transfer]);
    return promise;
  }

  async loadCueMol() {
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
    
    try {
      await this.invokeWorker('init-cuemol', load_path);
      console.log('init cuemol OK');
    } catch (e) {
      console.log('init cuemol ERROR!!!', e);
    }
  }

  async createScene() {
    return await this.invokeWorker('create-scene-view', 'Scene 1', 'View 1');
  }

  async bindCanvas(canvas, view_id, dpr) {
    const offscreen = canvas.transferControlToOffscreen();
    return await this.invokeWorkerWithTransfer('bind-canvas', offscreen, view_id, dpr);
  }

  async loadTestPDB(scene_id, view_id) {
    return await this.invokeWorker('load-test-pdb', scene_id, view_id);
  }

  resized(view_id, w, h, dpr) {
    this._worker.postMessage(['resized', view_id, w, h, dpr]);
  }

  onMouseEvent(view_id, method, event) {
    const { clientX, clientY, screenX, screenY, buttons } = event;
    const ev = { clientX, clientY, screenX, screenY, buttons };
    this._worker.postMessage([method, view_id, ev]);
  }

  async addEventListener(aCatStr, aSrcType, aEvtType, aSrcID, aObs) {
    const slot_id = await this.invokeWorker('add-event-listener',
                                            aCatStr, aSrcType, aEvtType, aSrcID);
    console.log("event listener registered: <"+aCatStr+">, id="+slot_id);
    this._slot[slot_id.toString()] = aObs;
    return slot_id;
  }

  removeEventListener(nID) {
    this.invokeWorker('remove-event-listener', nID);
    // console.log("EventManager, unload slot: "+nID);
    delete this._slot[nID.toString()];
  }

  async startLogger() {
    return await this.invokeWorker('start-logger');
  }

  eventNotify(slot, category, srcCat, evtType, srcUID, evtStr) {
    let json = null;
    let jobj = null;

    // console.log('notify called:', slot, category, srcCat, evtType, srcUID, evtStr);
    
    if (typeof evtStr === 'string') {
      json = evtStr;
      if (json && json.length>0)
        jobj = JSON.parse(json);
      else
        jobj = new Object();
    }
    else {
      // TODO: impl??
      // let cm = require("cuemol");
      // jobj = cm.convPolymObj(evtStr);
      // dd("Event notify arg4=obj, "+jobj);
      console.log('unknown evtStr type', evtStr);
    }

    const dict_args = {
      method: category,
      srcCat: srcCat,
      evtType: evtType,
      srcUID: srcUID,
      obj: jobj,
      // raw: args,
    };

    const strslot = slot.toString();
    if (strslot in this._slot) {
      const obs = this._slot[strslot];
      if (typeof obs === "function")
        return obs(dict_args);
      else if ("notify" in obs && typeof obs.notify === "function")
        return obs.notify(dict_args);
      else
        console.log("warning : event for slot "+strslot+" is not delivered!!");
    }
    return null;
  }
};

export const cuemol_worker = new CueMolWorker();
