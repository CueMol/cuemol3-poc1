
#include <common.h>

#include "TestDLRenderer.hpp"

#include <gfx/DisplayContext.hpp>
#include <gfx/Hittest.hpp>
#include <gfx/SolidColor.hpp>

namespace qsys {

using gfx::DisplayContext;
using gfx::SolidColor;

TestDLRenderer::TestDLRenderer() : super_t()
{
    resetAllProps();
}

TestDLRenderer::~TestDLRenderer()
{
    LOG_DPRINTLN("TestDLRenderer(%p) destructed\n", this);
}

LString TestDLRenderer::toString() const
{
    return LString::format("TestDLRenderer(name=%s)", getName().c_str());
}

////////////////////////////////////////////

const char *TestDLRenderer::getTypeName() const
{
    return "dltest";
}

void TestDLRenderer::renderTriangles(DisplayContext *pdl)
{
    pdl->startTriangles();

    pdl->color(1, 0, 0);
    pdl->vertex(3, 3, 0);
    pdl->color(0, 1, 0);
    pdl->vertex(13, 3, 0);
    pdl->color(0, 0, 1);
    pdl->vertex(13, 13, 10);

    pdl->color(0, 1, 1);
    pdl->vertex(0, -10, -5);
    pdl->color(1, 0, 1);
    pdl->vertex(-3, 0, 12);
    pdl->color(1, 1, 0);
    pdl->vertex(1, -13, 1);

    pdl->end();
}

void TestDLRenderer::renderLines(DisplayContext *pdl)
{
    const float del = 10;
    pdl->startLines();

    pdl->color(0, 1, 0);
    pdl->vertex(del, 0, 0);
    pdl->vertex(del, del, 0);

    pdl->color(0, 1, 0);
    pdl->vertex(0, 0, 0);
    pdl->vertex(0, del, 0);
    // pdl->color(1, 0, 1);
    // pdl->vertex(0, 0, 0);
    // pdl->vertex(0, -del, 0);

    pdl->color(1, 0, 0);
    pdl->vertex(del, 0, 0);
    pdl->vertex(del+del, 0, 0);

    pdl->color(1, 0, 0);
    pdl->vertex(0, 0, 0);
    pdl->vertex(del, 0, 0);
    // pdl->color(0, 1, 1);
    // pdl->vertex(0, 0, 0);
    // pdl->vertex(-del, 0, 0);

    // pdl->color(0, 0, 1);
    // pdl->vertex(0, 0, 0);
    // pdl->vertex(0, 0, del);
    // pdl->color(1, 1, 0);
    // pdl->vertex(0, 0, 0);
    // pdl->vertex(0, 0, -del);
    pdl->end();
}

void TestDLRenderer::renderLineStrip(DisplayContext *pdl)
{
    const float rad = 10.0;

    pdl->startLineStrip();
    pdl->color(0, 1, 0);

    for (int i=0; i<360; ++i) {
        const float th = qlib::toRadian(float(i));
        const float x = ::cosf(th) * rad;
        const float y = ::sinf(th) * rad;
        pdl->vertex(x, y, 0);
    }
    pdl->end();

    pdl->startLineStrip();
    pdl->color(1, 0, 1);
    for (int i=0; i<360; i+=10) {
        const float th = qlib::toRadian(float(i));
        const float x = ::cosf(th) * rad;
        const float y = ::sinf(th) * rad;
        pdl->vertex(0, x, y);
    }
    pdl->end();
}

void TestDLRenderer::renderTriangleStrip(DisplayContext *pdl)
{
    pdl->startTriangleStrip();

    pdl->color(1, 0, 0);
    pdl->vertex(3, 3, 0);
    pdl->color(0, 1, 0);
    pdl->vertex(13, 3, 0);
    pdl->color(0, 0, 1);
    pdl->vertex(13, 13, 10);

    pdl->color(0, 1, 1);
    pdl->vertex(0, -10, -5);
    pdl->color(1, 0, 1);
    pdl->vertex(-3, 0, 12);
    pdl->color(1, 1, 0);
    pdl->vertex(1, -13, 1);

    pdl->end();
}

void TestDLRenderer::render(DisplayContext *pdl)
{
    printf("TestDLRenderer::render called %p\n", pdl);
    // renderTriangles(pdl);
    // renderLines(pdl);
    // renderLineStrip(pdl);
    renderTriangleStrip(pdl);
    printf("TestDLRenderer::render OK\n");
}


Vector4D TestDLRenderer::getCenter() const
{
    return Vector4D(0, 0, 0);
}
}  // namespace qsys
