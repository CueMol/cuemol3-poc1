#include <common.h>

#include "ElecView.hpp"

#include "ElecDisplayContext.hpp"

namespace node_jsbr {

ElecView::ElecView() : m_pCtxt(new ElecDisplayContext()) {}

ElecView::ElecView(const ElecView &r) {}

ElecView::~ElecView() {}

//////////

LString ElecView::toString() const
{
    return LString("ElecView");
}

gfx::DisplayContext *ElecView::getDisplayContext()
{
    return m_pCtxt;
}

/// Setup the projection matrix for stereo (View interface)
void ElecView::setUpModelMat(int nid)
{
    super_t::setUpModelMat(nid);

    auto peer = m_peerObjRef.Value();
    auto env = peer.Env();
    auto method = peer.Get("setUpModelMat").As<Napi::Function>();
    method.Call(peer, {m_modelArrayBuf.Value()});

    // printf("setUpModelMat OK\n");
}

/// Setup projection matrix (View interface)
void ElecView::setUpProjMat(int cx, int cy)
{
    super_t::setUpProjMat(cx, cy);

    // TODO: impl
    // glFogf(GL_FOG_START, (GLfloat)fognear);
    // glFogf(GL_FOG_END, (GLfloat)fogfar);
    // setFogColorImpl();

    float *pbuf = &m_projMat.ai(1);

    constexpr size_t buf_size = 4 * 4;
    auto peer = m_peerObjRef.Value();
    auto env = peer.Env();
    auto method = peer.Get("setUpProjMat").As<Napi::Function>();
    method.Call(peer, {Napi::Number::New(env, m_bcx), Napi::Number::New(env, m_bcy),
                       m_projArrayBuf.Value()});

    resetProjChgFlag();

    // printf("setUpProjMat OK\n");
}

/// Draw current scene
void ElecView::drawScene()
{
    qsys::ScenePtr pScene = getScene();
    if (pScene.isnull()) {
        MB_DPRINTLN("DrawScene: invalid scene %d !!", getSceneID());
        return;
    }

    gfx::DisplayContext *pdc = getDisplayContext();
    pdc->setCurrent();

    if (isProjChange()) setUpProjMat(-1, -1);
    setUpModelMat(MM_NORMAL);

    gfx::ColorPtr pBgCol = pScene->getBgColor();
    clear(pBgCol);

    try {
        pScene->display(pdc);
    } catch (const std::exception &e) {
        printf("uncaught exception %s\n", e.what());
    } catch (...) {
        printf("uncaught exception!!!\n");
        throw;
    }
}

void ElecView::bindPeer(Napi::Object peer)
{
    printf("ElecView::bindPeer called\n");
    m_peerObjRef = Napi::Persistent(peer);
    auto env = peer.Env();

    constexpr size_t buf_size = 4 * 4;
    {
        float *pbuf = &m_modelMat.ai(1);
        Napi::Object array_buf = Napi::ArrayBuffer::New(
            env, pbuf, buf_size * sizeof(float), [](Napi::Env, void *finalizeData) {
                printf("finalizer called for %p\n", finalizeData);
                // delete [] static_cast<float*>(finalizeData);
            });
        m_modelArrayBuf = Napi::Persistent(array_buf);
    }

    {
        float *pbuf = &m_projMat.ai(1);
        Napi::Object array_buf = Napi::ArrayBuffer::New(
            env, pbuf, buf_size * sizeof(float), [](Napi::Env, void *finalizeData) {
                printf("finalizer called for %p\n", finalizeData);
                // delete [] static_cast<float*>(finalizeData);
            });
        m_projArrayBuf = Napi::Persistent(array_buf);
    }

    // init();

    setUpProjMat(-1, -1);
    // setUpLightColor();
    m_pCtxt->init(this);
}

void ElecView::clear(const gfx::ColorPtr &col)
{
    auto peer = m_peerObjRef.Value();
    auto env = peer.Env();
    auto method = peer.Get("clear").As<Napi::Function>();
    method.Call(peer,
                {Napi::Number::New(env, col->fr()), Napi::Number::New(env, col->fg()),
                 Napi::Number::New(env, col->fb())});
}

void registerViewFactory()
{
    qsys::View::setViewFactory(MB_NEW ElecViewFactory());
}

}  // namespace node_jsbr
