#include "ElecDisplayContext.hpp"

#include <napi.h>

#include <gfx/DrawAttrArray.hpp>
#include <qsys/SceneManager.hpp>
#include <qsys/style/StyleMgr.hpp>

// #include "ElecDisplayList.hpp"
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
    file_names.set("vertex", "%%CONFDIR%%/shaders/vertex_shader.glsl");
    file_names.set("fragment", "%%CONFDIR%%/shaders/fragment_shader.glsl");

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
    pImpl->drawBuffer(m_pView, data.isUpdated(), m_bEnableLighting);
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

void ElecDisplayContext::setLighting(bool f)
{
    m_bEnableLighting = f;
}

void ElecDisplayContext::setMaterial(const LString &name)
{
    super_t::setMaterial(name);

    // TODO: check changed (and only update if changed)
    qsys::StyleMgr *pSM = qsys::StyleMgr::getInstance();
    float dvalue;

    // Default Material: (plastic-like shading)
    //  Ambient = 0.2 (*(1,1,1))
    //  Diffuse = 0.8
    //  Specular = 0.4
    float amb = 0.2, diff = 0.8, spec = 0.4;
    float shin = 32.0;

    dvalue = pSM->getMaterial(name, gfx::Material::MAT_AMBIENT);
    if (dvalue >= -0.1) {
        amb = dvalue;
    }

    dvalue = pSM->getMaterial(name, gfx::Material::MAT_DIFFUSE);
    if (dvalue >= -0.1) {
        diff = dvalue;
    }

    dvalue = pSM->getMaterial(name, gfx::Material::MAT_SPECULAR);
    if (dvalue >= -0.1) {
        spec = dvalue;
    }

    dvalue = pSM->getMaterial(name, gfx::Material::MAT_SHININESS);
    if (dvalue >= -0.1) {
        shin = dvalue;
    }

    // GLfloat tmpv[4] = {0.0, 0.0, 0.0, 1.0};
    // tmpv[0] = tmpv[1] = tmpv[2] = float(amb);
    // glLightfv(GL_LIGHT0, GL_AMBIENT, tmpv);
    // tmpv[0] = tmpv[1] = tmpv[2] = float(diff);
    // glLightfv(GL_LIGHT0, GL_DIFFUSE, tmpv);
    // tmpv[0] = tmpv[1] = tmpv[2] = float(spec);
    // glLightfv(GL_LIGHT0, GL_SPECULAR, tmpv);
    // glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, float(shin));
}

}  // namespace node_jsbr
