#include <common.h>

#include "wrapper.hpp"

#include <emscripten.h>

#include <qlib/ClassRegistry.hpp>
#include <qlib/LScriptable.hpp>
#include <qlib/LString.hpp>
#include <qlib/LVarArgs.hpp>
#include <qlib/LVariant.hpp>

namespace embr {

using qlib::LString;

std::string Wrapper::toString() const
{
    auto pobj = getWrapped();
    if (!pobj) {
        return "embr::Wrapper (null)";
    }

    auto msg = qlib::LString::format("embr::Wrapper (%p) <%s>", pobj,
                                     pobj->toString().c_str());
    return msg.c_str();
}

em::val Wrapper::getProp(const std::string &propname)
{
    auto pScObj = getWrapped();
    if (!pScObj) {
        // TODO: throw js excpt
        LOG_DPRINTLN("Wrapped obj is null");
        return em::val::undefined();
        ;
    }

    if (!pScObj->hasNestedProperty(propname)) {
        auto msg =
            LString::format("GetProp: property \"%s\") not found.", propname.c_str());
        LOG_DPRINTLN(msg);
        // TODO: throw js excpt
        return em::val::undefined();
        ;
    }

    qlib::LVariant lvar;
    if (!pScObj->getNestedProperty(propname, lvar)) {
        LString msg = LString::format("GetProp: getProperty(\"%s\") call failed.",
                                      propname.c_str());
        LOG_DPRINTLN(msg);
        // TODO: throw js excpt
        return em::val::undefined();
        ;
    }

    return lvarToEmVal(lvar);
}

void Wrapper::setProp(const std::string &propname, const em::val &value)
{
    auto pScObj = getWrapped();
    if (!pScObj) {
        // TODO: throw js excpt
        LOG_DPRINTLN("Wrapped obj is null");
        return;
    }

    if (!pScObj->hasNestedProperty(propname)) {
        auto msg =
            LString::format("SetProp: property \"%s\") not found.", propname.c_str());
        LOG_DPRINTLN(msg);
        // TODO: throw js excpt
        return;
    }

    // variant (lvar) doesn't have ownership of its content
    qlib::LVariant lvar;
    // lvar.setRealValue(1.0);
    if (!emValToLVar(value, lvar)) {
        auto msg = LString::format(
            "SetProp(%s): Cannot converting em::val to LVariant.", propname.c_str());
        LOG_DPRINTLN(msg);
        // TODO: throw js excpt
        return;
    }

    if (!pScObj->setNestedProperty(propname, lvar)) {
        auto msg = LString::format("SetProp(%s) failed.", propname.c_str());
        LOG_DPRINTLN(msg);
        // TODO: throw js excpt
        return;
    }
}

em::val Wrapper::invokeMethod(const std::string &methodname, const em::val &args)
{
    auto pScObj = getWrapped();
    if (!pScObj) {
        LOG_DPRINTLN("Wrapped obj is null");
        return em::val::undefined();
    }

    if (!args.isArray()) {
        LOG_DPRINTLN("args is not array");
        return em::val::undefined();
    }

    // Convert argments
    const size_t nargs = args["length"].as<size_t>();
    qlib::LVarArgs largs(nargs);

    for (size_t i = 0; i < nargs; ++i) {
        if (!emValToLVar(args[i], largs.at(i))) {
            auto msg = LString::format("call method %s: cannot convert arg %d",
                                       methodname.c_str(), i);
            LOG_DPRINTLN(msg);
            // TODO: throw js excpt
            return em::val::undefined();
        }
    }

    LOG_DPRINTLN("invoke method %s nargs=%zu", methodname.c_str(), nargs);

    // Invoke method
    bool ok = false;
    LString errmsg;

    try {
        ok = pScObj->invokeMethod(methodname, largs);
        if (!ok) errmsg = LString::format("call method %s: failed", methodname.c_str());
    } catch (qlib::LException &e) {
        errmsg = LString::format("Exception occured in native method %s: %s",
                                 methodname.c_str(), e.getMsg().c_str());
    } catch (std::exception &e) {
        errmsg = LString::format("Std::exception occured in native method %s: %s",
                                 methodname.c_str(), e.what());
    } catch (...) {
        LOG_DPRINTLN("*********");
        errmsg = LString::format("Unknown Exception occured in native method %s",
                                 methodname.c_str());
    }

    if (!ok) {
        LOG_DPRINTLN(errmsg);
        // TODO: throw js excpt
        return em::val::undefined();
    }

    // Convert returned value
    return lvarToEmVal(largs.retval());
}

em::val Wrapper::lvarToEmVal(qlib::LVariant &variant)
{
    switch (variant.getTypeID()) {
        case qlib::LVariant::LT_NULL:
            return em::val::null();

        case qlib::LVariant::LT_BOOLEAN:
            return em::val(variant.getBoolValue());

        case qlib::LVariant::LT_INTEGER:
            return em::val(variant.getIntValue());

        case qlib::LVariant::LT_REAL:
            return em::val(variant.getRealValue());

        case qlib::LVariant::LT_STRING:
            return em::val(variant.getStringValue().c_str());

        case qlib::LVariant::LT_OBJECT: {
            auto pObj = createWrapper(variant.getObjectPtr());
            // At this point, the ownership of value is passed to PyObject
            //  (forget() avoids deleting the ptr transferred to PyObject)
            variant.forget();
            return em::val(pObj);
        }
        case qlib::LVariant::LT_ARRAY: {
            // TODO: impl
            break;
        }
        case qlib::LVariant::LT_DICT: {
            // TODO: impl
            break;
        }
        default:
            break;
    }

    auto msg = LString::format("Unable to convert LVariant(%d) to em::val",
                               variant.getTypeID());
    LOG_DPRINTLN(msg);
    return em::val::undefined();
}

/// convert emscripten::val to LVariant
bool Wrapper::emValToLVar(const em::val &value, qlib::LVariant &rvar)
{
    try {
        // null
        if (value.isNull() || value.isUndefined()) {
            rvar.setNull();
            return true;
        }
        // boolean
        if (value.isTrue()) {
            rvar.setBoolValue(true);
            return true;
        }
        if (value.isFalse()) {
            rvar.setBoolValue(false);
            return true;
        }
        // number
        if (value.isNumber()) {
            rvar.setRealValue(value.as<double>());
            return true;
        }
        // string
        if (value.isString()) {
            rvar.setStringValue(value.as<std::string>().c_str());
            return true;
        }
        // function (callable)
        if (value.instanceof (value.global("Function"))) {
            // TODO: impl
            printf("function not supported\n");
            return false;
        }
        // array
        if (value.isArray()) {
            // TODO: impl
            printf("array not supported\n");
            return false;
        }
        // object
        {
            // try to get wrapped scrobj
            auto pwrapper = value.as<WrapperPtr>();
            if (!pwrapper) {
                printf("Object not wrapper object\n");
                return false;
            }

            auto pScrObj = pwrapper->getWrapped();
            if (!pScrObj) {
                printf("Null wrapped object\n");
                return false;
            }
            if (pScrObj) {
                // pobj is owned by the interpreter (?)
                // (variant share the ptr and won't have the ownership!!)
                rvar.shareObjectPtr(pScrObj);
                return true;
            }
        }
    } catch (...) {
        printf("Unknown exception\n");
        return false;
    }

    printf("Unsupported value type\n");
    return false;
}

WrapperPtr Wrapper::createWrapper(qlib::LScriptable *pObj)
{
    Wrapper *pWrapper = new Wrapper();
    pWrapper->setWrapped(pObj);
    // return pWrapper;
    return WrapperPtr(pWrapper);
}

qlib::LScriptable *Wrapper::createObj(const char *clsname)
{
    qlib::LClass *pCls = NULL;
    try {
        qlib::ClassRegistry *pMgr = qlib::ClassRegistry::getInstance();
        MB_ASSERT(pMgr != NULL);
        pCls = pMgr->getClassObj(clsname);
        MB_DPRINTLN("!!! CreateObj, LClass for %s: %p", clsname, pCls);
    } catch (...) {
        LString msg = LString::format("createObj class %s not found", clsname);
        LOG_DPRINTLN("Error: %s", msg.c_str());
        return nullptr;
    }

    qlib::LDynamic *pDyn = pCls->createScrObj();

    if (!pDyn) {
        LString msg = LString::format(
            "createObj %s failed (class.createScrObj returned NULL)", clsname);
        LOG_DPRINTLN("Error: %s", msg.c_str());
        return nullptr;
    }

    MB_DPRINTLN("createObj(%s) OK, result=%p!!", clsname, pDyn);

    return static_cast<qlib::LScriptable *>(pDyn);
}

}  // namespace embr

