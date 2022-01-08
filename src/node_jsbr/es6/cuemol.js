import { wrapper_map } from './wrapper_loader';

export class CueMol {
  constructor(myapi) {
    this._internal = myapi.internal;
  }

  get internal() {
    return this._internal;
  }

  initCueMol(config) {
    this.internal.initCueMol(config);
  }

  bindView(id, view) {}

  createWrapper(native_obj) {
    if (typeof native_obj === 'undefined') {
      return null;
    }
    // console.log('native_obj:', native_obj);
    const className = native_obj.getClassName();
    // console.log('className:', className);
    const Klass = wrapper_map[className];
    const obj = new Klass(native_obj, this);
    // console.log('wrapper created:', obj);
    return obj;
  }

  createObj(className) {
    const obj = this.internal.createObj(className);
    return this.createWrapper(obj);
  }

  getService(className) {
    const obj = this.internal.getService(className);
    return this.createWrapper(obj);
  }
}
