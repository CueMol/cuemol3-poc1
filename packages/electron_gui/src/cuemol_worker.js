
const { getAppPathInfo } = window.myAPI;

function makeMethodSeq(method, seqno) {
  return method + '.' + seqno.toString();
}

class CueMolWorker {

  constructor() {
    this._seqno = 0;
    console.log('launch worker...');
    this._worker = new Worker('./worker.js');
    console.log('launch worker OK', this._worker);

    this._worker_onmessage_dict = {};

    this._worker.onmessage = (event) => {
      if (event.data[0] === 'event-notify') {
        const [, ...evtargs] = event.data;
        try {
          this.eventNotify(...evtargs);
        } catch (e) {
          console.log('event manager notify failed:', e);
        }
        return;
      }
      const [method, seqno, ...args] = event.data;
      const method_seq = makeMethodSeq(method, seqno);

      if (method_seq in this._worker_onmessage_dict) {
        this._worker_onmessage_dict[method_seq].apply(this, args);
        delete this._worker_onmessage_dict[method_seq];
      }
    };

    this._slot = {};
  }

  postMessage(method, seq, args, xfer=null) {
    if (xfer === null)
      this._worker.postMessage([method, seq, ...args]);
    else
      this._worker.postMessage([method, seq, ...args], [xfer]);
  }

  getSeqNo() {
    this._seqno ++;
    return this._seqno; 
  }

  addListener(method, seqno, handler) {
    const method_seq = makeMethodSeq(method, seqno);
    this._worker_onmessage_dict[method_seq] = handler;
  }

  async invokeWorker(method, ...args) {
    const cur_seq = this.getSeqNo(); 
    let promise = new Promise((resolve, reject) => {
      this.addListener(method, cur_seq, (result, ...msgargs) => {
        if (result) {
          resolve(msgargs);
        } else {
          reject(msgargs);
        }
      });
    });
    this.postMessage(method, cur_seq, args);
    return promise;
  }

  async invokeWorkerWithTransfer(method, transfer, ...args) {
    const cur_seq = this.getSeqNo(); 
    let promise = new Promise((resolve, reject) => {
      this.addListener(method, cur_seq, (result, ...msgargs) => {
        if (result) {
          resolve(msgargs);
        } else {
          reject(msgargs);
        }
      });
    });
    this.postMessage(method, cur_seq, args, transfer);
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

  async getSceneByView(view_id) {
    const [scene_id] = await this.invokeWorker('get-scene-by-view', view_id);
    return scene_id;
  }

  async getSceneData(scene_id) {
    const [data] = await this.invokeWorker('get-scene-data', scene_id);
    return data;
  }

  async bindCanvas(canvas, view_id, dpr) {
    const offscreen = canvas.transferControlToOffscreen();
    return await this.invokeWorkerWithTransfer('bind-canvas', offscreen, offscreen, view_id, dpr);
  }

  async addView(canvas_id, view_id) {
    return await this.invokeWorker('add-view', canvas_id, view_id);
  }

  async loadTestPDB(scene_id, view_id) {
    return await this.invokeWorker('load-test-pdb', scene_id, view_id);
  }

  resized(view_id, w, h, dpr) {
    const cur_seq = this.getSeqNo(); 
    this.postMessage('resized', cur_seq, [view_id, w, h, dpr]);
  }

  onMouseEvent(view_id, method, event) {
    const { clientX, clientY, screenX, screenY, buttons } = event;
    const ev = { clientX, clientY, screenX, screenY, buttons };
    const cur_seq = this.getSeqNo(); 
    this.postMessage(method, cur_seq, [view_id, ev]);
  }

  async addEventListener(aCatStr, aSrcType, aEvtType, aSrcID, aObs) {
    const [slot_id] = await this.invokeWorker('add-event-listener',
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

  async openPDBFile(scene_id, file_path) {
    return await this.invokeWorker('open-pdb-file', scene_id, file_path);
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
