#pragma once

#include "DispListRenderer.hpp"
#include "qsys.hpp"

namespace qsys {

class QSYS_API TestDLRenderer : public DispListRenderer
{
    MC_SCRIPTABLE;
    MC_CLONEABLE;

private:
    typedef DispListRenderer super_t;

public:
    TestDLRenderer();
    TestDLRenderer(const TestDLRenderer &r) : super_t(r) {}
    virtual ~TestDLRenderer();

    //////////////////////////////////////////////////////
    // Renderer implementation

    virtual void render(DisplayContext *pdl);

    virtual const char *getTypeName() const;
    virtual LString toString() const;
    virtual qlib::Vector4D getCenter() const;

    virtual bool isCompatibleObj(ObjectPtr pobj) const
    {
        return true;
    }

    virtual void unloading() {}

    virtual bool isHitTestSupported() const
    {
        return false;
    }

    void renderTriangles(DisplayContext *pdl);
    void renderTriangleStrip(DisplayContext *pdl);

    void renderLines(DisplayContext *pdl);
    void renderLineStrip(DisplayContext *pdl);
};

}  // namespace qsys

