#pragma once

#include <gfx/DisplayContext.hpp>
#include <emscripten/html5.h>

#include "embr.hpp"

namespace embr {

class EmProgramObject;
class EmView;

class EmDisplayContext : public gfx::DisplayContext
{
private:
    typedef gfx::DisplayContext super_t;

    /// UID of the target view
    qlib::uid_t m_nViewID;

    /// UID of the target scene
    qlib::uid_t m_nSceneID;

    EmView *m_pView;

    /// Default program object (shader)
    EmProgramObject *m_pDefPO;

    /// matrix stack
    // TODO: remove?
    std::deque<qlib::Matrix4D> m_matstack;

    qlib::LString m_sectionName;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_ctxt;
    
public:
    EmDisplayContext() : m_pView(nullptr), m_pDefPO(nullptr) {}
    virtual ~EmDisplayContext();

    void init(EmView *pView);
    void attach(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctxt) { m_ctxt = ctxt; }

    inline qlib::uid_t getViewID() const { return m_nViewID; }
    inline qlib::uid_t getSceneID() const { return m_nSceneID; }

    virtual void setTargetView(qsys::View *pView);

    virtual void drawElem(const gfx::AbstDrawElem &data);

    virtual void startSection(const qlib::LString &section_name);

    virtual void endSection();

    virtual gfx::DisplayContext *createDisplayList();
    virtual bool canCreateDL() const;
    virtual void callDisplayList(DisplayContext *pdl);
    virtual bool isCompatibleDL(DisplayContext *pdl) const;

    //

    virtual bool setCurrent();
    virtual bool isCurrent() const;
    virtual bool isFile() const;

    virtual void vertex(const qlib::Vector4D &);
    virtual void normal(const qlib::Vector4D &);
    virtual void color(const gfx::ColorPtr &c);

    virtual void pushMatrix();
    virtual void popMatrix();
    virtual void multMatrix(const qlib::Matrix4D &mat);
    virtual void loadMatrix(const qlib::Matrix4D &mat);

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

    void clearMatStack()
    {
        m_matstack.erase(m_matstack.begin(), m_matstack.end());
    }

    void xform_vec(Vector4D &v) const
    {
        const Matrix4D &mtop = m_matstack.front();
        v.w() = 1.0;
        mtop.xform4D(v);
    }

    void xform_norm(Vector4D &v) const
    {
        const Matrix4D &mtop = m_matstack.front();
        v.w() = 0.0;
        mtop.xform4D(v);
    }

    ///////////////////////////////
    // OpenGL SL support

  public:
    /// Create the GLSL program object.
    /// If program object with the same name already exists, returns it.
    /// @param name name of the program objec.
    /// @return program object having the specified name.
    EmProgramObject *createProgramObject(const LString &name);

    /// Get the GLSL program object by name.
    /// @param name name of the program object.
    /// @return program object having the specified name.
    EmProgramObject *getProgramObject(const LString &name);

};

}  // namespace embr
