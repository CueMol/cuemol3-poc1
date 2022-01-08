#include <common.h>

#include <emscripten.h>

#include <qlib/ClassRegistry.hpp>
#include <qlib/LScriptable.hpp>
#include <qlib/LString.hpp>
#include <qlib/LVarArgs.hpp>
#include <qlib/LVariant.hpp>

// #include "wrapper.hpp"

namespace embr {
bool initCueMol(const std::string &config);
}

extern "C"
{
    using EMBR_HANDLE = void *;

    EMSCRIPTEN_KEEPALIVE bool initCueMol(const char *config)
    {
        return embr::initCueMol(config);
    }

    EMSCRIPTEN_KEEPALIVE EMBR_HANDLE createObject(const char *className)
    {
        qlib::LClass *pCls = nullptr;
        try {
            qlib::ClassRegistry *pMgr = qlib::ClassRegistry::getInstance();
            MB_ASSERT(pMgr != NULL);
            pCls = pMgr->getClassObj(className);
            MB_DPRINTLN("CreateObj, LClass for %s: %p", className, pCls);
        } catch (...) {
            auto msg = qlib::LString::format("createObj class %s not found", className);
            LOG_DPRINTLN("Error: %s", msg.c_str());
            return nullptr;
        }

        auto pDyn = pCls->createScrObj();

        if (!pDyn) {
            auto msg = qlib::LString::format(
                "createObj %s failed (class.createScrObj returned NULL)", className);
            LOG_DPRINTLN("Error: %s", msg.c_str());
            return nullptr;
        }

        MB_DPRINTLN("createObj(%s) OK, result=%p!!", className, pDyn);

        // return static_cast<qlib::LScriptable *>(pDyn);
        return static_cast<EMBR_HANDLE>(pDyn);
    }

    EMSCRIPTEN_KEEPALIVE EMBR_HANDLE getService(const char *className)
    {
        qlib::LDynamic *pObj = nullptr;
        try {
            qlib::ClassRegistry *pMgr = qlib::ClassRegistry::getInstance();
            MB_ASSERT(pMgr != NULL);
            pObj = pMgr->getSingletonObj(className);
        } catch (...) {
            auto msg = qlib::LString::format("getService(%s) failed", className);
            LOG_DPRINTLN("Error: %s", msg.c_str());
            return nullptr;
        }

        if (pObj == nullptr) {
            auto msg =
                qlib::LString::format("getService(%s) returned nullptr", className);
            LOG_DPRINTLN("Error: %s", msg.c_str());
            return nullptr;
        }

        return static_cast<EMBR_HANDLE>(pObj);
    }

    EMSCRIPTEN_KEEPALIVE void destroyObject(EMBR_HANDLE pobj)
    {
        auto pScObj = static_cast<qlib::LScriptable *>(pobj);
        if (pScObj == nullptr) {
            // TODO: throw js excpt
            LOG_DPRINTLN("Wrapped obj is null");
            return;
        }
        pScObj->destruct();
    }

    EMSCRIPTEN_KEEPALIVE const char *getClassName(EMBR_HANDLE pobj)
    {
        auto pScObj = static_cast<qlib::LScriptable *>(pobj);
        if (pScObj == nullptr) {
            // TODO: throw js excpt
            LOG_DPRINTLN("Wrapped obj is null");
            return nullptr;
        }

        qlib::LClass *pCls = pScObj->getClassObj();
        if (pCls == nullptr) {
            // TODO: throw js excpt
            LOG_DPRINTLN("Wrapped obj's class is null");
            return nullptr;
        }
        return pCls->getClassName();
    }

    EMSCRIPTEN_KEEPALIVE void getProp(EMBR_HANDLE pobj, const char *propname,
                                      EMBR_HANDLE presult)
    {
        auto pScObj = static_cast<qlib::LScriptable *>(pobj);
        if (!pScObj) {
            // TODO: throw js excpt
            LOG_DPRINTLN("Wrapped obj is null");
            return;
        }

        if (!pScObj->hasNestedProperty(propname)) {
            auto msg =
                qlib::LString::format("GetProp: property \"%s\") not found.", propname);
            LOG_DPRINTLN(msg);
            return;
        }

        auto pVarArgs = static_cast<qlib::LVarArgs *>(presult);

        auto &lvar = pVarArgs->retval();

        if (!pScObj->getNestedProperty(propname, lvar)) {
            auto msg = qlib::LString::format(
                "GetProp: getProperty(\"%s\") call failed.", propname);
            LOG_DPRINTLN(msg);
            return;
        }

        // printf("getProp(%s) OK\n", propname);
        // if (lvar.isReal()) {
        //     printf("result=%f\n", lvar.getRealValue());
        // }
    }

    EMSCRIPTEN_KEEPALIVE void setProp(EMBR_HANDLE pobj, const char *propname,
                                      EMBR_HANDLE pargs)
    {
        auto pScObj = static_cast<qlib::LScriptable *>(pobj);
        if (!pScObj) {
            // TODO: throw js excpt
            LOG_DPRINTLN("Wrapped obj is null");
            return;
        }

        if (!pScObj->hasNestedProperty(propname)) {
            auto msg =
                qlib::LString::format("SetProp: property \"%s\") not found.", propname);
            LOG_DPRINTLN(msg);
            // TODO: throw js excpt
            return;
        }

        auto pVarArgs = static_cast<qlib::LVarArgs *>(pargs);
        // variant (lvar) doesn't have ownership of its content
        auto &lvar = pVarArgs->at(0);

        if (!pScObj->setNestedProperty(propname, lvar)) {
            auto msg = qlib::LString::format("SetProp(%s) failed.", propname);
            LOG_DPRINTLN(msg);
            // TODO: throw js excpt
            return;
        }
    }

    EMSCRIPTEN_KEEPALIVE void invokeMethod(EMBR_HANDLE pobj, const char *methodname,
                                           EMBR_HANDLE pargs)
    {
        auto pScObj = static_cast<qlib::LScriptable *>(pobj);
        if (!pScObj) {
            LOG_DPRINTLN("Wrapped obj is null");
            // TODO: throw js excpt
            return;
        }

        if (!pScObj->hasMethod(methodname)) {
            auto msg = qlib::LString::format("InvokeMethod: method \"%s\" not found.",
                                             methodname);
            LOG_DPRINTLN(msg);
            // TODO: throw js excpt
            return;
        }

        auto pVarArgs = static_cast<qlib::LVarArgs *>(pargs);

        // Invoke method
        bool ok = false;
        qlib::LString errmsg;

        try {
            ok = pScObj->invokeMethod(methodname, *pVarArgs);
            if (!ok) {
                errmsg = qlib::LString::format("call method %s: failed", methodname);
            }
        } catch (qlib::LException &e) {
            errmsg = qlib::LString::format("Exception occured in native method %s: %s",
                                           methodname, e.getMsg().c_str());
        } catch (std::exception &e) {
            errmsg = qlib::LString::format(
                "Std::exception occured in native method %s: %s", methodname, e.what());
        } catch (...) {
            errmsg = qlib::LString::format(
                "Unknown Exception occured in native method %s", methodname);
        }

        if (!ok) {
            LOG_DPRINTLN(errmsg);
            // TODO: throw js excpt
            return;
        }
    }

    EMSCRIPTEN_KEEPALIVE EMBR_HANDLE allocVarArgs(size_t nargs)
    {
        // printf("alloc_varargs called %zu\n", nargs);
        auto result = new qlib::LVarArgs(nargs);
        // printf("alloc_varargs result %p\n", result);
        return static_cast<EMBR_HANDLE>(result);
    }

    EMSCRIPTEN_KEEPALIVE void freeVarArgs(EMBR_HANDLE ptr)
    {
        // printf("free_varargs called %p\n", ptr);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        delete pVarArgs;
    }

    EMSCRIPTEN_KEEPALIVE int getVarArgsTypeID(EMBR_HANDLE ptr, int ind)
    {
        // printf("getVarArgsTypeID called %p [%d] \n", ptr, ind);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            return pVarArgs->at(ind).getTypeID();
        } else {
            return pVarArgs->retval().getTypeID();
        }
    }

    //

    EMSCRIPTEN_KEEPALIVE void setBoolVarArgs(EMBR_HANDLE ptr, int ind, bool value)
    {
        // printf("setBoolVarargs called %p [%d] <- %d\n", ptr, ind, value);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            pVarArgs->at(ind).setBoolValue(value);
        } else {
            pVarArgs->retval().setBoolValue(value);
        }
    }

    EMSCRIPTEN_KEEPALIVE bool getBoolVarArgs(EMBR_HANDLE ptr, int ind)
    {
        // printf("getBoolVarargs called %p [%d] \n", ptr, ind);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            return pVarArgs->at(ind).getBoolValue();
        } else {
            return pVarArgs->retval().getBoolValue();
        }
    }

    //

    EMSCRIPTEN_KEEPALIVE void setIntVarArgs(EMBR_HANDLE ptr, int ind, int value)
    {
        // printf("setIntVarargs called %p [%d] <- %d\n", ptr, ind, value);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            pVarArgs->at(ind).setIntValue(value);
        } else {
            pVarArgs->retval().setIntValue(value);
        }
    }

    EMSCRIPTEN_KEEPALIVE int getIntVarArgs(EMBR_HANDLE ptr, int ind)
    {
        // printf("getIntVarargs called %p [%d] \n", ptr, ind);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            return pVarArgs->at(ind).getIntValue();
        } else {
            return pVarArgs->retval().getIntValue();
        }
    }

    //

    EMSCRIPTEN_KEEPALIVE void setRealVarArgs(EMBR_HANDLE ptr, int ind, double value)
    {
        // printf("setRealVarargs called %p [%d] <- %f\n", ptr, ind, value);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            pVarArgs->at(ind).setRealValue(value);
        } else {
            pVarArgs->retval().setRealValue(value);
        }
    }

    EMSCRIPTEN_KEEPALIVE int getRealVarArgs(EMBR_HANDLE ptr, int ind)
    {
        // printf("getRealVarargs called %p [%d] \n", ptr, ind);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            return pVarArgs->at(ind).getRealValue();
        } else {
            return pVarArgs->retval().getRealValue();
        }
    }

    //

    EMSCRIPTEN_KEEPALIVE void setStrVarArgs(EMBR_HANDLE ptr, int ind, const char *value)
    {
        // printf("setStrVarArgs called %p [%d] <- %s\n", ptr, ind, value);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            pVarArgs->at(ind).setStringValue(value);
        } else {
            pVarArgs->retval().setStringValue(value);
        }
    }

    EMSCRIPTEN_KEEPALIVE const char *getStrVarArgs(EMBR_HANDLE ptr, int ind)
    {
        // printf("getStrVarArgs called %p [%d] \n", ptr, ind);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            return pVarArgs->at(ind).getStringValue().c_str();
        } else {
            return pVarArgs->retval().getStringValue().c_str();
        }
    }

    //

    EMSCRIPTEN_KEEPALIVE void setObjectVarArgs(EMBR_HANDLE ptr, int ind,
                                               EMBR_HANDLE value)
    {
        // printf("setObjectVarArgs called %p [%d] <- %p\n", ptr, ind, value);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        auto &&variant = (ind >= 0) ? pVarArgs->at(ind) : pVarArgs->retval();

        auto pObj = static_cast<qlib::LScriptable *>(value);

        // pobj is owned by the interpreter-side
        // (variant share the ptr and won't have the ownership!!)
        variant.shareObjectPtr(pObj);
    }

    EMSCRIPTEN_KEEPALIVE EMBR_HANDLE getObjectVarArgs(EMBR_HANDLE ptr, int ind)
    {
        // printf("getObjectVarArgs called %p [%d] \n", ptr, ind);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);

        EMBR_HANDLE pobj;
        auto &&variant = (ind >= 0) ? pVarArgs->at(ind) : pVarArgs->retval();
        pobj = variant.getObjectPtr();

        // At this point, the ownership of value is passed to interpreter-side
        //  (forget() avoids deleting the ptr transferred to interpreter-side)
        variant.forget();

        return pobj;
    }
}
