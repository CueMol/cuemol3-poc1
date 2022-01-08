#pragma once

#include <napi.h>

#include <qlib/MatrixND.hpp>
#include <qsys/EsView.hpp>
#include <qsys/MouseEventHandler.hpp>
#include <qsys/Scene.hpp>
#include <qsys/qsys.hpp>

namespace node_jsbr {

class ElecDisplayContext;

class ElecView : public qsys::EsView
{
    MC_SCRIPTABLE;
    using super_t = qsys::EsView;

private:
    ElecDisplayContext *m_pCtxt;

    /// JS-side WebGL display manager
    Napi::ObjectReference m_peerObjRef;

    Napi::ObjectReference m_modelArrayBuf, m_projArrayBuf;

public:
    ElecView();

    ElecView(const ElecView &r);

    virtual ~ElecView();

    //////////

public:
    virtual LString toString() const;

    virtual gfx::DisplayContext *getDisplayContext();

    /// Setup the projection matrix for stereo (View interface)
    virtual void setUpModelMat(int nid);

    /// Setup projection matrix (View interface)
    virtual void setUpProjMat(int w, int h);

    /// Draw current scene
    virtual void drawScene();

    //////////

    void bindPeer(Napi::Object peer);

    inline Napi::Object getPeerObj()
    {
        return m_peerObjRef.Value();
    }

private:
    void clear(const gfx::ColorPtr &col);

};

class ElecViewFactory : public qsys::ViewFactory
{
public:
    ElecViewFactory() {}
    virtual ~ElecViewFactory() {}
    virtual qsys::View *create()
    {
        return MB_NEW ElecView();
    }
};

void registerViewFactory();

}  // namespace node_jsbr
