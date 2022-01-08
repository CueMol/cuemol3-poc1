#pragma once

#include <gfx/ProgramObject.hpp>
#include <qlib/MapTable.hpp>

namespace node_jsbr {

class ElecView;

class ElecProgramObject : public gfx::ProgramObject
{
private:
    using super_t = gfx::ProgramObject;
    ElecView *m_pView;

    qlib::LString loadFile(const qlib::LString &filename);

public:
    ElecProgramObject(ElecView *pView) : m_pView(pView) {}
    virtual ~ElecProgramObject();

    virtual bool loadShaders(const qlib::MapTable<qlib::LString> &name);

    virtual void enable();

    virtual void disable();
};
}  // namespace node_jsbr
