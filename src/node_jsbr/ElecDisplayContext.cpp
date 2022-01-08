#include "ElecDisplayContext.hpp"

#include <napi.h>

#include <gfx/DrawAttrArray.hpp>
#include <qsys/SceneManager.hpp>

#include "ElecDisplayList.hpp"
#include "ElecProgramObject.hpp"
#include "ElecVBOImpl.hpp"
#include "ElecView.hpp"

namespace node_jsbr {

ElecDisplayContext::~ElecDisplayContext()
{
    if (m_pDefPO) delete m_pDefPO;
}

void ElecDisplayContext::init(ElecView *pView)
{
    m_pView = pView;
    setTargetView(pView);

    m_pDefPO = static_cast<ElecProgramObject *>(createProgramObject("default"));

    qlib::MapTable<qlib::LString> file_names;
    file_names.set("vertex", "shaders/vertex_shader.glsl");
    file_names.set("fragment", "shaders/fragment_shader.glsl");

    m_pDefPO->loadShaders(file_names);
}

void ElecDisplayContext::drawElem(const gfx::AbstDrawElem &data)
{
    auto pda = static_cast<const gfx::AbstDrawAttrs *>(&data);
    auto pImpl = static_cast<ElecVBOImpl *>(data.getVBO());
    if (!pImpl) {
        auto name = LString::format("%s_%p", getSectionName().c_str(), pda);
        pImpl = new ElecVBOImpl(m_pView, name, *pda);
        data.setVBO(pImpl);
    }
    pImpl->drawBuffer(m_pView, data.isUpdated());
    // printf("ElecDisplayContext::drawElem\n");
    data.setUpdated(false);
}

void ElecDisplayContext::startSection(const qlib::LString &section_name)
{
    super_t::startSection(section_name);
    if (!m_pDefPO) return;
    m_pDefPO->enable();
    // m_pDefPO->setUniformF("frag_alpha", getAlpha());
}

void ElecDisplayContext::endSection()
{
    super_t::endSection();
    if (!m_pDefPO) return;
    // m_pDefPO->setUniformF("frag_alpha", 1.0);
    m_pDefPO->disable();
}

bool ElecDisplayContext::setCurrent()
{
    return true;
}
bool ElecDisplayContext::isCurrent() const
{
    return true;
}

gfx::ProgramObject *ElecDisplayContext::createProgObjImpl()
{
    return MB_NEW ElecProgramObject(m_pView);
}

//////////

void ElecDisplayContext::vertex(const qlib::Vector4D &) {}
void ElecDisplayContext::normal(const qlib::Vector4D &) {}
void ElecDisplayContext::color(const gfx::ColorPtr &c) {}

void ElecDisplayContext::setPolygonMode(int id) {}
void ElecDisplayContext::startPoints() {}
void ElecDisplayContext::startPolygon() {}
void ElecDisplayContext::startLines() {}
void ElecDisplayContext::startLineStrip() {}
void ElecDisplayContext::startTriangles() {}
void ElecDisplayContext::startTriangleStrip() {}
void ElecDisplayContext::startTriangleFan() {}
void ElecDisplayContext::startQuadStrip() {}
void ElecDisplayContext::startQuads() {}
void ElecDisplayContext::end() {}

}  // namespace node_jsbr
