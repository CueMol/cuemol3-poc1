
#include <common.h>

#include "TestRenderer.hpp"

#include <gfx/DisplayContext.hpp>
#include <gfx/Hittest.hpp>
#include <gfx/SolidColor.hpp>

using namespace qsys;
using gfx::DisplayContext;
using gfx::SolidColor;

TestRenderer::TestRenderer() : Renderer(), m_pDrawData(nullptr), m_bUpdate(false)
{
    resetAllProps();
}

TestRenderer::~TestRenderer()
{
    if (m_pDrawData) delete m_pDrawData;
    LOG_DPRINTLN("TestRenderer(%p) destructed\n", this);
}

LString TestRenderer::toString() const
{
    return LString::format("TestRenderer(name=%s)", getName().c_str());
}

////////////////////////////////////////////

const char *TestRenderer::getTypeName() const
{
    return "test";
}

void TestRenderer::display(DisplayContext *pdc)
{
    if (!m_pDrawData) {
        m_pDrawData = new DrawArray();
        m_pDrawData->setAttrSize(2);
        m_pDrawData->setAttrInfo(0, 0, 3, qlib::type_consts::QTC_FLOAT32,
                                 offsetof(DrawAttr, x));
        m_pDrawData->setAttrInfo(1, 1, 4, qlib::type_consts::QTC_FLOAT32,
                                 offsetof(DrawAttr, r));
        m_pDrawData->alloc(VERTEX_NUMS);
        initData();
    }

    if (m_bUpdate)
        updateData();
    pdc->drawElem(*m_pDrawData);
}

// void TestRenderer::displayHit(DisplayContext *pdc)
// {
//     display(pdc);
// }

// LString TestRenderer::interpHit(const gfx::RawHitData &rhit)
// {
//   LString rval;
//   int nface;
//   if (rhit.getData(4, nface)) {
//     rval += LString::format("\"faceid\": %d,\n", nface);
//   }
//   rval += "\"objtype\": \"Test\",\n";
//   return rval;
// }

Vector4D TestRenderer::getCenter() const
{
    return Vector4D(0, 0, 0);
}
