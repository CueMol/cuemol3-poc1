const path = require('path');

class Utils {
    constructor(module) {
        this.module = module;
    }

    convStr(value) {
        let arr = this.module.intArrayFromString(value);
        let ptr = this.module.allocate(arr, this.module.ALLOC_NORMAL);
        return ptr;
    }

    initCueMol(config) {
        let pstr = this.convStr(config);
        let pobj = this.module["_initCueMol"](pstr);
        this.module._free(pstr);
        return pobj
    }

    createWrapper(pobj) {
        let pstr = this.module["_getClassName"](pobj);
        let className = this.module.UTF8ToString(pstr);
        console.log(`createWrapper: className=${className}`);
        // let moduleFile = path.join(WRAPPERS_DIR, className);
        let moduleFile = path.join(__dirname, "wrappers", className);
        console.log(`module file: ${moduleFile}`);
        const Klass = require(moduleFile);
        return new Klass(pobj, this);
    }

    createObject(className) {
        let pstr = this.convStr(className);
        let pobj = this.module["_createObject"](pstr);
        this.module._free(pstr);
        return this.createWrapper(pobj);
    }
    
    getService(className) {
        let pstr = this.convStr(className);
        let pobj = this.module["_getService"](pstr);
        this.module._free(pstr);
        return this.createWrapper(pobj);
    }

    varToVal(args, ind) {
        let type_id = this.module["_getVarArgsTypeID"](args, ind);
        console.log(`varToVal ${ind} -> type ID ${type_id}`);
        if (type_id == 0) {
            // LT_NULL
            return null;
        }
        else if (type_id == 1) {
            // LT_BOOLEAN
            return this.module["_getBoolVarArgs"](args, ind);
        }
        else if (type_id == 2) {
            // LT_INTEGER
            return this.module["_getIntVarArgs"](args, ind);
        }
        else if (type_id == 3) {
            // LT_REAL
            return this.module["_getRealVarArgs"](args, ind);
        }
        else if (type_id == 4) {
            // LT_STRING
            let pstr = this.module["_getStrVarArgs"](args, ind);
            return this.module.UTF8ToString(pstr);
        }
        else if (type_id == 5) {
            // LT_OBJECT
            let pobj = this.module["_getObjectVarArgs"](args, ind);
            return this.createWrapper(pobj);
        }
        // else if (type_id == 6) {
        //     // LT_SMARTPTR
        // }
        else if (type_id == 7) {
            // LT_ENUM
        }
        else if (type_id == 8) {
            // LT_ARRAY
        }
        else if (type_id == 9) {
            // LT_LIST
        }
        else if (type_id == 10) {
            // LT_DICT
        }

        // error
        throw `unsupported type ID: ${type_id}`;
    }

    valToVar(args, ind, value) {
        if (value === null || value === undefined) {
            // LT_NULL
        }
        else if (typeof value === "boolean") {
            // LT_BOOLEAN
            this.module["_setBoolVarArgs"](args, ind, value);
        }
        else if (typeof value === "number") {
            // LT_REAL / LT_INT
            this.module["_setRealVarArgs"](args, ind, value);
        }
        else if (typeof value === "string") {
            // LT_STRING
            let pstr = this.convStr(value);
            try {
                this.module["_setStrVarArgs"](args, ind, pstr);
            }
            finally {
                this.module._free(pstr);
            }
        }
        else if (value instanceof BaseWrapper) {
            // LT_OBJECT
            this.module["_setObjectVarArgs"](args, ind, value.wrapped);
        }
        else {
            console.log(`unsupported type: ${typeof value}`);
        }
    }

}

module.exports = (path) => {
    console.log("load path:", path);
    embr = require(path);
    promise = embr();

    return new Promise((fulfilled, rejected) => {
        promise.then( (m) => { fulfilled(new Utils(m)); }, () => {rejected();});
    })
}

class BaseWrapper {
    constructor(aWrapped, aUtils) {
        this._wrapped = aWrapped;
        this._utils = aUtils;
    }

    get wrapped() {
        return this._wrapped;
    }

    get utils() {
        return this._utils;
    }

    get module() {
        return this._utils.module;
    }

    destroy() {
        this.module["_destroyObject"](this._wrapped);
        this._wrapped = undefined;
    }

    getProp(propName) {
        let args = this.module["_allocVarArgs"](0);
        let pPropName = this.utils.convStr(propName);
        try {
            this.module["_getProp"](this._wrapped, pPropName, args);
            return this.utils.varToVal(args, -1);
        }
        finally {
            this.module._free(pPropName);
            this.module["_freeVarArgs"](args);
        }
    }

    setProp(propName, value) {
        let args = this.module["_allocVarArgs"](1);
        let pPropName = this.utils.convStr(propName);
        try {
            this.utils.valToVar(args, 0, value);
            this.module["_setProp"](this._wrapped, pPropName, args);
        }
        finally {
            this.module._free(pPropName);
            this.module["_freeVarArgs"](args);
        }
    }

    invokeMethod(methodName, ...args) {
        let pMethodName = this.utils.convStr(methodName);
        let num_args = args.length;
        let varargs = this.module["_allocVarArgs"](num_args);
        try {
            args.forEach((elem, ind) => {
                this.utils.valToVar(varargs, ind, elem);
            });
            this.module["_invokeMethod"](this._wrapped, pMethodName, varargs);
            return this.utils.varToVal(varargs, -1);
        }
        finally {
            this.module._free(pMethodName);
            this.module["_freeVarArgs"](args);
        }
    }

    toString() {
        if (this._wrapped!==undefined)
            return `Wrapper(ptr=0x${this._wrapped.toString(16)})`
        else
            return `Wrapper(ptr=null)`
    }
}

module.exports.BaseWrapper = BaseWrapper;
