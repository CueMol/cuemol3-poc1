#pragma once
#include <qlib/MapTable.hpp>

#include "gfx.hpp"

namespace gfx {

class GFX_API ProgramObject
{
private:
    qlib::LString m_progObjName;

public:
    virtual ~ProgramObject() = default;

    virtual bool loadShaders(const qlib::MapTable<qlib::LString> &name) = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;

    inline void setName(const qlib::LString &name)
    {
        m_progObjName = name;
    }

    inline qlib::LString getName() const
    {
        return m_progObjName;
    }
};

}  // namespace gfx
