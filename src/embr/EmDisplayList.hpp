#pragma once

#include <gfx/DisplayContext.hpp>
#include <gfx/DrawAttrArray.hpp>

namespace embr {

class EmProgramObject;
class EmView;

class EmDisplayList : public gfx::DisplayContext
{
private:
    typedef gfx::DisplayContext super_t;

    /////
    // lines

    // TODO: use uint8 for colors
    struct LineDrawAttr
    {
        float x, y, z, w;
        float r, g, b, a;
    };

    using LineDrawArray = gfx::DrawAttrArray<LineDrawAttr>;
    LineDrawArray *m_pLineArray;

    using LineDrawBuf = std::deque<LineDrawAttr>;
    LineDrawBuf m_lineBuf;

    bool m_fPrevPosValid;

    /////
    // triangles

    struct TrigVertAttr
    {
        float x, y, z, w;
        // float nx, ny, nz, nw;
        float r, g, b, a;
    };
    
    using TrigVertArray = gfx::DrawAttrArray<TrigVertAttr>;
    TrigVertArray *m_pTrigArray;

    using TrigVertBuf = std::deque<TrigVertAttr>;
    TrigVertBuf m_trigBuf;

    /////

    bool m_fValid;

    /// matrix stack
    std::deque<qlib::Matrix4D> m_matstack;

    /// current color
    gfx::ColorPtr m_pColor;

    /// current normal vec
    qlib::Vector4D m_norm;

    /// current drawing mode
    int m_nDrawMode;

    Vector4D m_prevPos;
    qlib::quint32 m_prevCol;
    Vector4D m_prevNorm;

    static const int DRAWMODE_NONE = 0;
    static const int DRAWMODE_POINTS = 1;
    // static const int DRAWMODE_POLYGON = 2;
    static const int DRAWMODE_LINES = 3;
    static const int DRAWMODE_LINESTRIP = 4;
    static const int DRAWMODE_TRIGS = 5;
    static const int DRAWMODE_TRIGSTRIP = 6;
    static const int DRAWMODE_TRIGFAN = 7;

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

    /// Draw a single line segment from v1 to v2 to the output
    /// v1 and v2 should be transformed by matrix stack
    void drawLine(const Vector4D &v1, qlib::quint32 c1, const Vector4D &v2,
                  qlib::quint32 c2);

    qlib::uid_t getSceneID() const;

    void endLines();

    void addTrigVert(const Vector4D &v, const Vector4D &n, qlib::quint32 c);

public:

    static const int DSLOC_VERT_POS = 0;
    static const int DSLOC_VERT_COLOR = 1;
    static const int DSLOC_VERT_NORMAL = 1;

    EmDisplayList();
    virtual ~EmDisplayList();

    virtual bool setCurrent() { return true; }
    virtual bool isCurrent() const { return true; }
    virtual bool isFile() const { return false; } 

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
    virtual void startQuadStrip() {}
    virtual void startQuads() {}
    virtual void end();

    virtual gfx::DisplayContext *createDisplayList();
    virtual bool canCreateDL() const;
    virtual bool isValid() const { return m_fValid; }
    virtual bool isDisplayList() const;
    virtual bool recordStart();
    virtual void recordEnd();

    gfx::AbstDrawAttrs *getLineArray() const { return m_pLineArray; }
    gfx::AbstDrawAttrs *getTrigArray() const { return m_pTrigArray; }
};
}  // namespace node_jsbr
