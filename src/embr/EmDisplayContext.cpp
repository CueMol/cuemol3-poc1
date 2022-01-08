#include "EmDisplayContext.hpp"

#include <gfx/DrawAttrArray.hpp>
#include <qsys/SceneManager.hpp>

#include "EmDisplayList.hpp"
#include "EmProgObjMgr.hpp"
#include "EmProgramObject.hpp"
#include "EmVBO.hpp"
#include "EmView.hpp"

namespace embr {

using gfx::AbstDrawElem;
using gfx::DisplayContext;
using gfx::DrawElem;

EmDisplayContext::~EmDisplayContext()
{
    if (m_pDefPO) delete m_pDefPO;
}

void EmDisplayContext::setTargetView(qsys::View *pView)
{
    super_t::setTargetView(pView);
    m_nViewID = pView->getUID();
    m_nSceneID = pView->getSceneID();
}

void EmDisplayContext::init(EmView *pView)
{
    m_pView = pView;
    setTargetView(pView);

    m_pDefPO = createProgramObject("default");
    m_pDefPO->loadShader("vert", "/shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
    m_pDefPO->loadShader("frag", "/shaders/fragment_shader.glsl", GL_FRAGMENT_SHADER);
    m_pDefPO->link();

    m_pDefPO->enable();
    // m_pDefPO->setUniform("enable_lighting", 0);
    // m_pDefPO->setUniformF("frag_alpha", 1.0);
    const auto mvp_mat_index = glGetUniformBlockIndex(m_pDefPO->getHandle(), "mvp_matrix");
    glUniformBlockBinding(m_pDefPO->getHandle(), mvp_mat_index, EmView::MVP_MAT_LOC);
    m_pDefPO->disable();
}

void EmDisplayContext::drawElem(const gfx::AbstDrawElem &data)
{
    auto &ada = *static_cast<const gfx::AbstDrawAttrs *>(&data);
    EmVBORep *pRep = static_cast<EmVBORep *>(ada.getVBO());
    if (pRep == nullptr) {
        pRep = MB_NEW EmVBORep(m_pView, ada);
    }
    pRep->drawBuffer(ada);
}

void EmDisplayContext::startSection(const qlib::LString &section_name)
{
    m_sectionName = section_name;
    if (!m_pDefPO) return;
    m_pDefPO->enable();
}

void EmDisplayContext::endSection()
{
    m_sectionName = "";
    if (!m_pDefPO) return;
    // m_pDefPO->setUniformF("frag_alpha", 1.0);
    m_pDefPO->disable();
}

bool EmDisplayContext::setCurrent()
{
    if (isCurrent()) return true;

    if (!emscripten_webgl_make_context_current(m_ctxt)) return false;

    return true;
}

bool EmDisplayContext::isCurrent() const
{
    return (emscripten_webgl_get_current_context() == m_ctxt);
}
bool EmDisplayContext::isFile() const
{
    return false;
}

gfx::DisplayContext *EmDisplayContext::createDisplayList()
{
    EmDisplayList *pdl = MB_NEW EmDisplayList();

    // Targets the same view as this
    pdl->setTargetView(getTargetView());

    LOG_DPRINTLN("createDisplayList OK\n");
    return pdl;
}

bool EmDisplayContext::canCreateDL() const
{
    return true;
}

bool EmDisplayContext::isCompatibleDL(DisplayContext *pdl) const
{
    EmDisplayList *psrc = dynamic_cast<EmDisplayList *>(pdl);
    if (psrc == nullptr) return false;
    return true;
}

void EmDisplayContext::callDisplayList(DisplayContext *pdl)
{
    // printf("callDisplayList called\n");
    EmDisplayList *psrc = dynamic_cast<EmDisplayList *>(pdl);
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

    // printf("callDisplayList OK\n");
}

//////////

void EmDisplayContext::vertex(const qlib::Vector4D &) {}
void EmDisplayContext::normal(const qlib::Vector4D &) {}
void EmDisplayContext::color(const gfx::ColorPtr &c) {}

void EmDisplayContext::pushMatrix()
{
    if (m_matstack.size() <= 0) {
        Matrix4D m;
        m_matstack.push_front(m);
        return;
    }
    const Matrix4D &top = m_matstack.front();
    m_matstack.push_front(top);
}
void EmDisplayContext::popMatrix()
{
    if (m_matstack.size() <= 1) {
        LString msg("POVWriter> FATAL ERROR: cannot popMatrix()!!");
        LOG_DPRINTLN(msg);
        MB_THROW(qlib::RuntimeException, msg);
        return;
    }
    m_matstack.pop_front();
}
void EmDisplayContext::multMatrix(const qlib::Matrix4D &mat)
{
    Matrix4D top = m_matstack.front();
    top.matprod(mat);
    m_matstack.front() = top;

    // check unitarity
    // checkUnitary();
}
void EmDisplayContext::loadMatrix(const qlib::Matrix4D &mat)
{
    m_matstack.front() = mat;

    // check unitarity
    // checkUnitary();
}

void EmDisplayContext::setPolygonMode(int id) {}
void EmDisplayContext::startPoints() {}
void EmDisplayContext::startPolygon() {}
void EmDisplayContext::startLines() {}
void EmDisplayContext::startLineStrip() {}
void EmDisplayContext::startTriangles() {}
void EmDisplayContext::startTriangleStrip() {}
void EmDisplayContext::startTriangleFan() {}
void EmDisplayContext::startQuadStrip() {}
void EmDisplayContext::startQuads() {}
void EmDisplayContext::end() {}

EmProgramObject *EmDisplayContext::createProgramObject(const LString &name)
{
    EmProgObjMgr *pMgr = EmProgObjMgr::getInstance();
    return pMgr->createProgramObject(name, this);
}

EmProgramObject *EmDisplayContext::getProgramObject(const LString &name)
{
    EmProgObjMgr *pMgr = EmProgObjMgr::getInstance();
    return pMgr->getProgramObject(name, this);
}

}  // namespace embr
