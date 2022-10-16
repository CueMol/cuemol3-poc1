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
    bool m_bBound;

    ElecDisplayContext *m_pCtxt;

    /// JS-side WebGL display manager
    Napi::ObjectReference m_peerObjRef;

    /// Lighting array
    std::array<float, 8> m_lightArray;

    /// UBO buffers
    Napi::ObjectReference m_modelArrayBuf, m_projArrayBuf;
    Napi::ObjectReference m_lightArrayBuf;

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

    void setLighting(float amb, float diff, float spec, float shin) {
        m_lightArray[0] = amb;
        m_lightArray[1] = diff;
        m_lightArray[2] = spec;
        m_lightArray[3] = shin;
    }
    void setLightDir(const Vector4D &dir) {
        m_lightArray[4] = float(dir.x());
        m_lightArray[5] = float(dir.y());
        m_lightArray[6] = float(dir.z());
        m_lightArray[7] = 0.0f;
    }
    void updateLightingUBO();

private:
    void clear(const gfx::ColorPtr &col);

    void createModelMatArrayBuf(Napi::Env env);
    void createProjMatArrayBuf(Napi::Env env);
    void createLightArrayBuf(Napi::Env env);
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
