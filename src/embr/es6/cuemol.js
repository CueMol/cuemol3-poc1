import BaseWrapper from './base_wrapper';
import wrapper_map from './wrapper_loader';

export default class CueMol {
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

    bindView(id, view) {
    }

    createWrapper(pobj) {
        let pstr = this.module["_getClassName"](pobj);
        let className = this.module.UTF8ToString(pstr);
        console.log(`createWrapper: className=${className}`);
        // let moduleFile = path.join(WRAPPERS_DIR, className);
        // let moduleFile = path.join(__dirname, "wrappers", className);
        // console.log(`module file: ${moduleFile}`);
        // const Klass = require(moduleFile);
        const Klass = wrapper_map[className];
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
        // console.log(`varToVal ${ind} -> type ID ${type_id}`);
        if (type_id === 0) {
            // LT_NULL
            return null;
        }
        else if (type_id === 1) {
            // LT_BOOLEAN
            return this.module["_getBoolVarArgs"](args, ind);
        }
        else if (type_id === 2) {
            // LT_INTEGER
            return this.module["_getIntVarArgs"](args, ind);
        }
        else if (type_id === 3) {
            // LT_REAL
            return this.module["_getRealVarArgs"](args, ind);
        }
        else if (type_id === 4) {
            // LT_STRING
            let pstr = this.module["_getStrVarArgs"](args, ind);
            return this.module.UTF8ToString(pstr);
        }
        else if (type_id === 5) {
            // LT_OBJECT
            let pobj = this.module["_getObjectVarArgs"](args, ind);
            return this.createWrapper(pobj);
        }
        // else if (type_id === 6) {
        //     // LT_SMARTPTR
        // }
        else if (type_id === 7) {
            // LT_ENUM
        }
        else if (type_id === 8) {
            // LT_ARRAY
        }
        else if (type_id === 9) {
            // LT_LIST
        }
        else if (type_id === 10) {
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
