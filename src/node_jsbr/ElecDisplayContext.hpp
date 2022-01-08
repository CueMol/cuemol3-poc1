#pragma once

#include <qsys/EsDisplayContext.hpp>

#include "node_jsbr.hpp"

namespace node_jsbr {

class ElecProgramObject;
class ElecView;

class ElecDisplayContext : public qsys::EsDisplayContext
{
private:
    using super_t = qsys::EsDisplayContext;

    /// Target View
    ElecView *m_pView;

    /// Default program object (shader)
    ElecProgramObject *m_pDefPO;

public:
    ElecDisplayContext() : m_pView(nullptr), m_pDefPO(nullptr) {}
    virtual ~ElecDisplayContext();

    void init(ElecView *pView);

    virtual void drawElem(const gfx::AbstDrawElem &data);

    virtual void startSection(const qlib::LString &section_name);

    virtual void endSection();

    // virtual gfx::DisplayContext *createDisplayList();
    // virtual bool canCreateDL() const;
    // virtual bool isCompatibleDL(DisplayContext *pdl) const;
    // virtual void callDisplayList(DisplayContext *pdl);

    //

    virtual bool setCurrent();
    virtual bool isCurrent() const;
    // virtual bool isFile() const;

    virtual void vertex(const qlib::Vector4D &);
    virtual void normal(const qlib::Vector4D &);
    virtual void color(const gfx::ColorPtr &c);

    virtual void setPolygonMode(int id);
    virtual void startPoints();
    virtual void startPolygon();
    virtual void startLines();
    virtual void startLineStrip();
    virtual void startTriangles();
    virtual void startTriangleStrip();
    virtual void startTriangleFan();
    virtual void startQuadStrip();
    virtual void startQuads();
    virtual void end();

    //

    ///////////////////////////////
    // Shader support

    virtual gfx::ProgramObject *createProgObjImpl();
};

}  // namespace node_jsbr
