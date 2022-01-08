#include <common.h>

#include "EsDisplayContext.hpp"

#include <gfx/DrawAttrArray.hpp>

#include "SceneManager.hpp"

#include "EsDisplayList.hpp"
#include "ProgObjMgr.hpp"
// #include "EmProgramObject.hpp"
// #include "EmVBO.hpp"
#include "EsView.hpp"

namespace qsys {

using gfx::AbstDrawElem;
using gfx::DisplayContext;
using gfx::DrawElem;

EsDisplayContext::~EsDisplayContext()
{
}

void EsDisplayContext::setTargetView(qsys::View *pView)
{
    super_t::setTargetView(pView);
    m_nViewID = pView->getUID();
    m_nSceneID = pView->getSceneID();
}

void EsDisplayContext::startSection(const qlib::LString &section_name)
{
    m_sectionName = section_name;
}

void EsDisplayContext::endSection()
{
    m_sectionName = "";
}

bool EsDisplayContext::isFile() const
{
    return false;
}

bool EsDisplayContext::canCreateDL() const
{
    return true;
}

gfx::DisplayContext *EsDisplayContext::createDisplayList()
{
    EsDisplayList *pdl = MB_NEW EsDisplayList();

    // Targets the same view as this
    pdl->setTargetView(getTargetView());

    printf("createDisplayList OK\n");
    return pdl;
}

bool EsDisplayContext::isCompatibleDL(DisplayContext *pdl) const
{
    EsDisplayList *psrc = dynamic_cast<EsDisplayList *>(pdl);
    if (psrc == nullptr) return false;
    return true;
}

void EsDisplayContext::callDisplayList(DisplayContext *pdl)
{
    // printf("callDisplayList called\n");
    EsDisplayList *psrc = dynamic_cast<EsDisplayList *>(pdl);
    if (psrc == nullptr || !psrc->isValid()) return;

    // Lines
    auto *pLines = psrc->getLineArray();
    if (pLines != nullptr) {
        drawElem(*pLines);
    }

    // Triangles
    auto *pTrigs = psrc->getTrigArray();
    if (pTrigs != nullptr) {
        drawElem(*pTrigs);
    }

    // Trig mesh
    auto *pMesh = psrc->getTrigMesh();
    if (pMesh != nullptr) {
        drawElem(*pMesh);
    }
    
    // printf("callDisplayList OK\n");
}


//////////

void EsDisplayContext::vertex(const qlib::Vector4D &) {}
void EsDisplayContext::normal(const qlib::Vector4D &) {}
void EsDisplayContext::color(const gfx::ColorPtr &c) {}

void EsDisplayContext::pushMatrix()
{
    if (m_matstack.size() <= 0) {
        Matrix4D m;
        m_matstack.push_front(m);
        return;
    }
    const Matrix4D &top = m_matstack.front();
    m_matstack.push_front(top);
}
void EsDisplayContext::popMatrix()
{
    if (m_matstack.size() <= 1) {
        LString msg("POVWriter> FATAL ERROR: cannot popMatrix()!!");
        LOG_DPRINTLN(msg);
        MB_THROW(qlib::RuntimeException, msg);
        return;
    }
    m_matstack.pop_front();
}
void EsDisplayContext::multMatrix(const qlib::Matrix4D &mat)
{
    Matrix4D top = m_matstack.front();
    top.matprod(mat);
    m_matstack.front() = top;

    // check unitarity
    // checkUnitary();
}
void EsDisplayContext::loadMatrix(const qlib::Matrix4D &mat)
{
    m_matstack.front() = mat;

    // check unitarity
    // checkUnitary();
}

void EsDisplayContext::setPolygonMode(int id) {}
void EsDisplayContext::startPoints() {}
void EsDisplayContext::startPolygon() {}
void EsDisplayContext::startLines() {}
void EsDisplayContext::startLineStrip() {}
void EsDisplayContext::startTriangles() {}
void EsDisplayContext::startTriangleStrip() {}
void EsDisplayContext::startTriangleFan() {}
void EsDisplayContext::startQuadStrip() {}
void EsDisplayContext::startQuads() {}
void EsDisplayContext::end() {}

ProgramObject *EsDisplayContext::createProgramObject(const LString &name)
{
    auto pMgr = ProgObjMgr::getInstance();
    auto ppo = pMgr->getProgramObject(name, getSceneID());
    if (ppo != nullptr) {
        return ppo;
    }
    ppo = createProgObjImpl();
    pMgr->registerProgramObject(name, getSceneID(), ppo);
    return ppo;
}

ProgramObject *EsDisplayContext::getProgramObject(const LString &name)
{
    auto pMgr = ProgObjMgr::getInstance();
    return pMgr->getProgramObject(name, getSceneID());
}

}  // namespace qsys
