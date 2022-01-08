#pragma once

#include <emscripten/val.h>

namespace qlib {
class LScriptable;
class LVariant;
}  // namespace qlib

namespace embr {

class Wrapper;
using WrapperPtr = std::shared_ptr<Wrapper>;
namespace em = emscripten;

class Wrapper
{
private:
    /// wrapped object
    qlib::LScriptable *m_pWrapped;

public:
    Wrapper() {}
    ~Wrapper() = default;

    qlib::LScriptable *getWrapped()
    {
        return m_pWrapped;
    }

    const qlib::LScriptable *getWrapped() const
    {
        return m_pWrapped;
    }

    void setWrapped(qlib::LScriptable *pval)
    {
        m_pWrapped = pval;
    }

    std::string toString() const;
    em::val getProp(const std::string &propname);
    void setProp(const std::string &propname, const em::val &value);
    em::val invokeMethod(const std::string &methodname, const em::val &args);

    static WrapperPtr createWrapper(qlib::LScriptable *pObj);

    static qlib::LScriptable *createObj(const char *clsname);

private:
    bool emValToLVar(const em::val &value, qlib::LVariant &rvar);
    em::val lvarToEmVal(qlib::LVariant &rvar);
};

}  // namespace embr
