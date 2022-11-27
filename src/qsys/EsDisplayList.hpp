#pragma once

#include <gfx/DisplayContext.hpp>
#include <gfx/DrawAttrArray.hpp>
#include <gfx/GrowMesh.hpp>

namespace qsys {

// TODO: Move to gfx module??
struct LineDrawAttr
{
    float x, y, z, w;
    // float r, g, b, a;
    qlib::quint8 r, g, b, a;
};
using LineDrawArray = gfx::DrawAttrArray<LineDrawAttr>;

// TODO: Move to gfx module??
struct TrigVertAttr
{
    float x, y, z, w;
    float nx, ny, nz, nw;
    qlib::quint8 r, g, b, a;
};
using TrigVertArray = gfx::DrawAttrArray<TrigVertAttr>;
// using TrigVertElems = gfx::DrawAttrElems<qlib::quint32, TrigVertAttr>;
// Non-fixed pipeline version of DrawElemVNCI
class TrigVertElems: public gfx::DrawAttrElems<qlib::quint32, TrigVertAttr>
{
public:
    using super_t = gfx::DrawAttrElems<qlib::quint32, TrigVertAttr>;
    using index_t = qlib::quint32;

    bool color(int ind, qlib::quint32 cc)
    {
        if (ind < 0 || getSize() <= ind) return false;

        // uint8 color
        super_t::at(ind).r = gfx::getRCode(cc);
        super_t::at(ind).g = gfx::getGCode(cc);
        super_t::at(ind).b = gfx::getBCode(cc);
        super_t::at(ind).a = gfx::getACode(cc);

        return true;
    }
    bool vertex(int ind, const qlib::Vector4D &v)
    {
        if (ind < 0 || getSize() <= ind) return false;
        super_t::at(ind).x = qfloat32(v.x());
        super_t::at(ind).y = qfloat32(v.y());
        super_t::at(ind).z = qfloat32(v.z());
        super_t::at(ind).w = qfloat32(v.w());  // 1.0f;
        return true;
    }
    bool normal(int ind, const qlib::Vector4D &v)
    {
        if (ind < 0 || getSize() <= ind) return false;
        super_t::at(ind).nx = qfloat32(v.x());
        super_t::at(ind).ny = qfloat32(v.y());
        super_t::at(ind).nz = qfloat32(v.z());
        super_t::at(ind).nw = 1.0f;
        return true;
    }

    /// set face index for triangles mode (shortcut method)
    void setIndex3(int ind, index_t n1, index_t n2, index_t n3)
    {
        // MB_ASSERT((ind * 3 + 2) < m_nIndSize);
        super_t::atind(ind * 3 + 0) = n1;
        super_t::atind(ind * 3 + 1) = n2;
        super_t::atind(ind * 3 + 2) = n3;
    }

    bool getVertex(int ind, Vector4D &v) const
    {
        if (ind < 0 || getSize() <= ind) return false;
        v.x() = super_t::at(ind).x;
        v.y() = super_t::at(ind).y;
        v.z() = super_t::at(ind).z;
        return true;
    }

    void startIndexTriangles(int nverts, int nfaces);
};

//////////
// Display list impl/emulation using VBO/IBO
//
class EsDisplayList : public gfx::DisplayContext
{
private:
    using super_t = gfx::DisplayContext;

    //////////
    // lines
    LineDrawArray *m_pLineArray;

    using LineDrawBuf = std::deque<LineDrawAttr>;
    LineDrawBuf m_lineBuf;

    bool m_fPrevPosValid;

    //////////
    // triangles

    //////////
    // trigs (vert only)

    TrigVertArray *m_pTrigArray;
    using TrigVertBuf = std::deque<TrigVertAttr>;
    TrigVertBuf m_trigBuf;

    //////////
    // trig mesh (vert + indices)

    gfx::GrowMesh m_mesh;

    TrigVertElems *m_pTrigMesh;

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

    void createLineArray();
    void createTrigArray();
    void createTrigMesh();

public:
    // Attribute location ID
    // These values should coincide with the location layout qualifiers in the shader
    static const int DSLOC_VERT_POS = 0;
    static const int DSLOC_VERT_COLOR = 1;
    static const int DSLOC_VERT_NORMAL = 2;

    EsDisplayList();
    virtual ~EsDisplayList();

    virtual bool setCurrent()
    {
        return true;
    }
    virtual bool isCurrent() const
    {
        return true;
    }
    virtual bool isFile() const
    {
        return false;
    }

    virtual void vertex(const qlib::Vector4D &);
    virtual void normal(const qlib::Vector4D &);
    virtual void color(const gfx::ColorPtr &c);

    virtual void pushMatrix();
    virtual void popMatrix();
    virtual void multMatrix(const qlib::Matrix4D &mat);
    virtual void loadMatrix(const qlib::Matrix4D &mat);

    virtual void startPoints();
    virtual void startLines();
    virtual void startLineStrip();
    virtual void startTriangles();
    virtual void startTriangleStrip();
    virtual void startTriangleFan();
    virtual void end();

    // not implemented
    virtual void setPolygonMode(int id);
    virtual void startPolygon();
    virtual void startQuadStrip() {}
    virtual void startQuads() {}

    virtual gfx::DisplayContext *createDisplayList();
    virtual bool canCreateDL() const;
    virtual bool isValid() const
    {
        return m_fValid;
    }
    virtual bool isDisplayList() const;
    virtual bool recordStart();
    virtual void recordEnd();

    auto *getLineArray() const
    {
        return m_pLineArray;
    }
    auto *getTrigArray() const
    {
        return m_pTrigArray;
    }
    auto *getTrigMesh() const
    {
        return m_pTrigMesh;
    }
};
}  // namespace qsys
