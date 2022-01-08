// -*-Mode: C++;-*-
//
// OpenGLES3 program object manager
//

#pragma once

#include <qlib/SingletonBase.hpp>
#include <qsys/SceneEvent.hpp>

#include "embr.hpp"

namespace embr {

using qlib::LString;
class EmProgramObject;
class EmDisplayContext;

///
///  Program object manager
///
class EmProgObjMgr : public qlib::SingletonBase<EmProgObjMgr>,
                                public qsys::SceneEventListener
{
private:
    typedef std::map<LString, EmProgramObject *> data_t;

    data_t m_data;

public:
    EmProgObjMgr() {}
    ~EmProgObjMgr();

    EmProgramObject *createProgramObject(const LString &name, EmDisplayContext *pdc);
    EmProgramObject *getProgramObject(const LString &name, EmDisplayContext *pdc);

    virtual void sceneChanged(qsys::SceneEvent &ev);
};

}  // namespace embr

SINGLETON_BASE_DECL(embr::EmProgObjMgr);
