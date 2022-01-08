export default class BaseWrapper {
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
