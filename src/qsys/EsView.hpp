#pragma once

#include <qlib/MatrixND.hpp>

#include "MouseEventHandler.hpp"
// #include "Scene.hpp"
#include "View.hpp"
#include "qsys.hpp"

namespace qsys {

class ElecDisplayContext;
using Matrix4F = qlib::MatrixND<4, float>;

class EsView : public View
{
protected:
    Matrix4F m_modelMat, m_projMat;

    int m_bcx, m_bcy;

public:
    EsView();

    EsView(const EsView &r);

    virtual ~EsView();

    //////////

public:
    /// Setup the projection matrix for stereo (View interface)
    virtual void setUpModelMat(int nid);

    /// Setup projection matrix (View interface)
    virtual void setUpProjMat(int w, int h);

    void onMouseDown(double clientX, double clientY, double screenX, double screenY,
                     int modif);
    void onMouseUp(double clientX, double clientY, double screenX, double screenY,
                   int modif);
    void onMouseMove(double clientX, double clientY, double screenX, double screenY,
                     int modif);

    //////////

protected:
    static const int DME_MOUSE_DOWN = 0;
    static const int DME_MOUSE_MOVE = 1;
    static const int DME_MOUSE_UP = 2;
    static const int DME_WHEEL = 3;
    static const int DME_DBCHK_TIMEUP = 4;

    MouseEventHandler m_meh;

    void setupInDevEvent(double clientX, double clientY, double screenX, double screenY,
                         int modif, InDevEvent &ev);
    void dispatchMouseEvent(int nType, InDevEvent &ev);
};

}  // namespace qsys
