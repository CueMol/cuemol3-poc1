//
const Module = require("../../build.em/src/embr/embr");

const WRAPPERS_DIR = "./wrappers";

const convStr = (value) => {
    let arr = Module.intArrayFromString(value);
    let ptr = Module.allocate(arr, Module.ALLOC_NORMAL);
    return ptr;
}

module.exports.initCueMol = (config) => {
    let pstr = convStr(config);
    let pobj = Module["_initCueMol"](pstr);
    Module._free(pstr);
    return pobj
}

const createWrapper = (pobj) => {
    let pstr = Module["_getClassName"](pobj);
    let className = Module.UTF8ToString(pstr);
    console.log(`createWrapper: className=${className}`);
    let moduleFile = `${WRAPPERS_DIR}/${className}`;
    console.log(`module file: ${moduleFile}`);
    const Klass = require(moduleFile);
    return new Klass(pobj);
}

module.exports.createObject = (className) => {
    let pstr = convStr(className);
    let pobj = Module["_createObject"](pstr);
    Module._free(pstr);
    return createWrapper(pobj);
}

module.exports.getService = (className) => {
    let pstr = convStr(className);
    let pobj = Module["_getService"](pstr);
    Module._free(pstr);
    return createWrapper(pobj);
}

const varToVal = (args, ind) => {
    let type_id = Module["_getVarArgsTypeID"](args, ind);
    console.log(`varToVal ${ind} -> type ID ${type_id}`);
    if (type_id == 0) {
        // LT_NULL
        return null;
    }
    else if (type_id == 1) {
        // LT_BOOLEAN
        return Module["_getBoolVarArgs"](args, ind);
    }
    else if (type_id == 2) {
        // LT_INTEGER
        return Module["_getIntVarArgs"](args, ind);
    }
    else if (type_id == 3) {
        // LT_REAL
        return Module["_getRealVarArgs"](args, ind);
    }
    else if (type_id == 4) {
        // LT_STRING
        let pstr = Module["_getStrVarArgs"](args, ind);
        return Module.UTF8ToString(pstr);
    }
    else if (type_id == 5) {
        // LT_OBJECT
        let pobj = Module["_getObjectVarArgs"](args, ind);
        return createWrapper(pobj);
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

const valToVar = (args, ind, value) => {
    if (value === null || value === undefined) {
        // LT_NULL
    }
    else if (typeof value === "boolean") {
        // LT_BOOLEAN
        Module["_setBoolVarArgs"](args, ind, value);
    }
    else if (typeof value === "number") {
        // LT_REAL / LT_INT
        Module["_setRealVarArgs"](args, ind, value);
    }
    else if (typeof value === "string") {
        // LT_STRING
        let pstr = convStr(value);
        try {
            Module["_setStrVarArgs"](args, ind, pstr);
        }
        finally {
            Module._free(pstr);
        }
    }
    else if (value instanceof BaseWrapper) {
        // LT_OBJECT
        Module["_setObjectVarArgs"](args, ind, value.wrapped);
    }
    else {
        console.log(`unsupported type: ${typeof value}`);
    }
}

class BaseWrapper {
    constructor(aWrapped) {
        this._wrapped = aWrapped;
    }

    get wrapped() {
        return this._wrapped;
    }

    destroy() {
        Module["_destroyObject"](this._wrapped);
        this._wrapped = undefined;
    }

    getProp(propName) {
        let args = Module["_allocVarArgs"](0);
        let pPropName = convStr(propName);
        try {
            Module["_getProp"](this._wrapped, pPropName, args);
            return varToVal(args, -1);
        }
        finally {
            Module._free(pPropName);
            Module["_freeVarArgs"](args);
        }
    }

    setProp(propName, value) {
        let args = Module["_allocVarArgs"](1);
        let pPropName = convStr(propName);
        try {
            valToVar(args, 0, value);
            Module["_setProp"](this._wrapped, pPropName, args);
        }
        finally {
            Module._free(pPropName);
            Module["_freeVarArgs"](args);
        }
    }

    invokeMethod(methodName, ...args) {
        let pMethodName = convStr(methodName);
        let num_args = args.length;
        let varargs = Module["_allocVarArgs"](num_args);
        try {
            args.forEach((elem, ind) => {
                valToVar(varargs, ind, elem);
            });
            Module["_invokeMethod"](this._wrapped, pMethodName, varargs);
            return varToVal(varargs, -1);
        }
        finally {
            Module._free(pMethodName);
            Module["_freeVarArgs"](args);
        }
    }

}

module.exports.BaseWrapper = BaseWrapper;


