// -*-Mode: C++;-*-
//
// OpenGL program object manager
//

#include <common.h>

#include "ProgObjMgr.hpp"

#include <gfx/DisplayContext.hpp>
#include <gfx/ProgramObject.hpp>

#include "SceneEvent.hpp"
#include "SceneManager.hpp"

SINGLETON_BASE_IMPL(qsys::ProgObjMgr);

namespace qsys {

using gfx::DisplayContext;

ProgObjMgr::~ProgObjMgr()
{
    for (data_t::value_type &elem : m_data) {
        MB_DPRINTLN("ProgMgr> dtor() Warning: %s not removed", elem.first.c_str());
        if (elem.second != NULL) {
            delete elem.second;
        }
    }
}

bool ProgObjMgr::registerProgramObject(const LString &name, qlib::uid_t nSceneID,
                                       ProgramObject *ppo)
{
    qsys::ScenePtr pScene = qsys::SceneManager::getSceneS(nSceneID);
    if (pScene.isnull()) {
        // ERROR
        return false;
    }
    pScene->addListener(this);

    ppo->setName(name);
    LString key = LString::format("%s@%d", name.c_str(), nSceneID);
    m_data.insert(data_t::value_type(key, ppo));
    return true;
}

ProgramObject *ProgObjMgr::getProgramObject(const LString &name, qlib::uid_t nSceneID)
{
    auto key = LString::format("%s@%d", name.c_str(), nSceneID);

    data_t::const_iterator i = m_data.find(key);
    if (i == m_data.end()) return NULL;

    return i->second;
}

void ProgObjMgr::sceneChanged(SceneEvent &ev)
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
}  // namespace qsys
