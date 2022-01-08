// -*-Mode: C++;-*-
//
// OpenGL program object manager
//

#include <common.h>

#include "EmProgObjMgr.hpp"

#include <qsys/SceneEvent.hpp>
#include <qsys/SceneManager.hpp>

#include "EmDisplayContext.hpp"
#include "EmProgramObject.hpp"

SINGLETON_BASE_IMPL(embr::EmProgObjMgr);

namespace embr {

using gfx::DisplayContext;
using qsys::SceneEvent;

EmProgObjMgr::~EmProgObjMgr()
{
    for (data_t::value_type &elem : m_data) {
        MB_DPRINTLN("EmProgMgr> dtor() Warning: %s not removed", elem.first.c_str());
        if (elem.second != NULL) {
            delete elem.second;
        }
    }
}

EmProgramObject *EmProgObjMgr::createProgramObject(const LString &name,
                                                   EmDisplayContext *pdc)
{
    EmProgramObject *pRval = NULL;

    pRval = getProgramObject(name, pdc);
    if (pRval != NULL) return pRval;

    qsys::ScenePtr pScene = qsys::SceneManager::getSceneS(pdc->getSceneID());
    if (pScene.isnull()) {
        // ERROR
        return NULL;
    }
    pScene->addListener(this);

    pRval = MB_NEW EmProgramObject();
    if (!pRval->init()) {
        delete pRval;
        return NULL;
    }

    LString key = LString::format("%s@%d", name.c_str(), pdc->getSceneID());
    m_data.insert(data_t::value_type(key, pRval));
    return pRval;
}

EmProgramObject *EmProgObjMgr::getProgramObject(const LString &name,
                                                EmDisplayContext *pdc)
{
    LString key = LString::format("%s@%d", name.c_str(), pdc->getSceneID());

    data_t::const_iterator i = m_data.find(key);
    if (i == m_data.end()) return NULL;

    return i->second;
}

void EmProgObjMgr::sceneChanged(SceneEvent &ev)
{
    if (ev.getType() != SceneEvent::SCE_SCENE_REMOVING) return;

    qlib::uid_t nid = ev.getTarget();
    LString key = LString::format("@%d", nid);

    std::list<LString> delkeys;

    for (data_t::value_type &elem : m_data) {
        if (elem.first.endsWith(key)) {
            delkeys.push_back(elem.first);
        }
    }

    for (const LString &key : delkeys) {
        data_t::iterator iter = m_data.find(key);
        if (iter != m_data.end()) {
            MB_DPRINTLN("Destroy progobj: %s", key.c_str());
            if (iter->second != NULL) delete iter->second;
            m_data.erase(iter);
        }
    }
}
}  // namespace embr
