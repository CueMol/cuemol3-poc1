#pragma once

#include <qlib/MatrixND.hpp>
#include <qsys/MouseEventHandler.hpp>
#include <qsys/Scene.hpp>
#include <qsys/View.hpp>
#include <qsys/qsys.hpp>

#include "embr.hpp"

namespace embr {

class EmDisplayContext;
// using Matrix4F = qlib::MatrixND<4, float>;

class EmView : public qsys::View
{
    MC_SCRIPTABLE;

private:
    EmDisplayContext *m_pCtxt;

    qlib::Matrix4D m_projMat, m_modelMat;

    // UBO for common uniforms (proj/model matrix)
    GLuint m_nComUBO;

    // sizes of the common uniforms
    static constexpr size_t SZ_PROJ_MAT = 4 * 4 * sizeof(float);
    static constexpr size_t SZ_MODEL_MAT = 4 * 4 * sizeof(float);

    uint8_t m_uboData[SZ_MODEL_MAT + SZ_PROJ_MAT];

    void setUboData(const Matrix4D &mat, size_t offset)
    {
        auto m = reinterpret_cast<float *>(&m_uboData[offset]);
        m[0] = mat.aij(1, 1);
        m[1] = mat.aij(2, 1);
        m[2] = mat.aij(3, 1);
        m[3] = mat.aij(4, 1);

        m[4] = mat.aij(1, 2);
        m[5] = mat.aij(2, 2);
        m[6] = mat.aij(3, 2);
        m[7] = mat.aij(4, 2);

        m[8] = mat.aij(1, 3);
        m[9] = mat.aij(2, 3);
        m[10] = mat.aij(3, 3);
        m[11] = mat.aij(4, 3);

        m[12] = mat.aij(1, 4);
        m[13] = mat.aij(2, 4);
        m[14] = mat.aij(3, 4);
        m[15] = mat.aij(4, 4);
    }

public:
    static constexpr int MVP_MAT_LOC = 0;

public:
    EmView();

    EmView(const EmView &r);

    virtual ~EmView();

    //////////

public:
    bool init();

    virtual LString toString() const;

    /// Setup the projection matrix for stereo (View interface)
    virtual void setUpModelMat(int nid);

    /// Setup projection matrix (View interface)
    virtual void setUpProjMat(int w, int h);

    /// Draw current scene
    virtual void drawScene();

    virtual gfx::DisplayContext *getDisplayContext();

    //////////

    void bind(const LString &id);

    void onMouseDown(double clientX, double clientY, double screenX, double screenY,
                     int modif);
    void onMouseUp(double clientX, double clientY, double screenX, double screenY,
                   int modif);
    void onMouseMove(double clientX, double clientY, double screenX, double screenY,
                     int modif);

private:
    qsys::MouseEventHandler m_meh;

    static const int DME_MOUSE_DOWN = 0;
    static const int DME_MOUSE_MOVE = 1;
    static const int DME_MOUSE_UP = 2;
    static const int DME_WHEEL = 3;
    static const int DME_DBCHK_TIMEUP = 4;

    void setupInDevEvent(double clientX, double clientY, double screenX, double screenY,
                         int modif, qsys::InDevEvent &ev);
    void dispatchMouseEvent(int nType, qsys::InDevEvent &ev);
};

class EmViewFactory : public qsys::ViewFactory
{
public:
    EmViewFactory() {}
    virtual ~EmViewFactory() {}
    virtual qsys::View *create()
    {
        return MB_NEW EmView();
    }
};

void registerViewFactory();

}  // namespace embr
