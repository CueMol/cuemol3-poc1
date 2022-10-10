#include <common.h>

#include "ElecView.hpp"

#include "ElecDisplayContext.hpp"

namespace node_jsbr {

ElecView::ElecView() : m_bBound(false), m_pCtxt(new ElecDisplayContext())
{
    printf("=== ElecView created (%p) ===\n", this);
}

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

    if (!m_bBound) {
        printf("ElecView::setUpModelMat> ElecView is not bound.\n");
        return;
    }

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

    if (!m_bBound) {
        printf("ElecView::setUpModelMat> ElecView is not bound.\n");
        return;
    }

    // TODO: impl
    // glFogf(GL_FOG_START, (GLfloat)fognear);
    // glFogf(GL_FOG_END, (GLfloat)fogfar);
    // setFogColorImpl();

    float *pbuf = &m_projMat.ai(1);

    constexpr size_t buf_size = 4 * 4;
    try {
        auto peer = m_peerObjRef.Value();
        auto env = peer.Env();
        auto method = peer.Get("setUpProjMat").As<Napi::Function>();
        method.Call(peer, {Napi::Number::New(env, m_bcx), Napi::Number::New(env, m_bcy),
                           m_projArrayBuf.Value()});
    } catch (const Napi::Error &e) {
        printf("=== Call jsmethod setUpProjMat failed. ===\n");
    } catch (...) {
        printf("=== Call jsmethod setUpProjMat failed. ===\n");
    }
    resetProjChgFlag();

    // printf("setUpProjMat OK\n");
}

void ElecView::updateLightingUBO()
{
    if (!m_bBound) {
        printf("ElecView::updateLightingUBO> ElecView is not bound.\n");
        return;
    }
    auto peer = m_peerObjRef.Value();
    auto env = peer.Env();
    auto method = peer.Get("setUpLight").As<Napi::Function>();
    method.Call(peer, {m_lightArrayBuf.Value()});
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

void ElecView::clear(const gfx::ColorPtr &col)
{
    if (!m_bBound) {
        printf("ElecView::setUpModelMat> ElecView is not bound.\n");
        return;
    }

    auto peer = m_peerObjRef.Value();
    auto env = peer.Env();
    auto method = peer.Get("clear").As<Napi::Function>();
    method.Call(peer,
                {Napi::Number::New(env, col->fr()), Napi::Number::New(env, col->fg()),
                 Napi::Number::New(env, col->fb())});
}

void ElecView::createModelMatArrayBuf(Napi::Env env)
{
    constexpr size_t MODEL_BUF_SIZE = 4 * 4 * sizeof(float);
    constexpr size_t NORM_BUF_SIZE = 3 * 3 * sizeof(float);

    float *pbuf = &m_modelMat.ai(1);
    Napi::Object array_buf = Napi::ArrayBuffer::New(
        env, pbuf, MODEL_BUF_SIZE + NORM_BUF_SIZE, [](Napi::Env, void *finalizeData) {
            printf("finalizer called for %p\n", finalizeData);
            // delete [] static_cast<float*>(finalizeData);
        });
    m_modelArrayBuf = Napi::Persistent(array_buf);
}

void ElecView::createProjMatArrayBuf(Napi::Env env)
{
    constexpr size_t PROJ_BUF_SIZE = 4 * 4 * sizeof(float);
    float *pbuf = &m_projMat.ai(1);
    Napi::Object array_buf = Napi::ArrayBuffer::New(
        env, pbuf, PROJ_BUF_SIZE, [](Napi::Env, void *finalizeData) {
            printf("finalizer called for %p\n", finalizeData);
            // delete [] static_cast<float*>(finalizeData);
        });
    m_projArrayBuf = Napi::Persistent(array_buf);
}

void ElecView::createLightArrayBuf(Napi::Env env)
{
    const size_t LIGHT_ARRAY_SIZE = m_lightArray.size() * sizeof(float);
    void *pbuf = m_lightArray.data();
    Napi::Object array_buf = Napi::ArrayBuffer::New(
        env, pbuf, LIGHT_ARRAY_SIZE, [](Napi::Env, void *finalizeData) {
            printf("finalizer called for %p\n", finalizeData);
            // delete [] static_cast<float*>(finalizeData);
        });
    m_lightArrayBuf = Napi::Persistent(array_buf);
}

void ElecView::bindPeer(Napi::Object peer)
{
    printf("ElecView::bindPeer called\n");
    m_peerObjRef = Napi::Persistent(peer);
    auto env = peer.Env();

    // Create UBOs
    createModelMatArrayBuf(env);
    createProjMatArrayBuf(env);
    createLightArrayBuf(env);

    m_bBound = true;

    setUpProjMat(-1, -1);
    setLighting(0.2f, 0.8f, 0.4f, 32.0f);
    setLightDir(Vector4D(1.0, 1.0, 1.5, 0.0));
    updateLightingUBO();

    m_pCtxt->init(this);
}

void registerViewFactory()
{
    qsys::View::setViewFactory(MB_NEW ElecViewFactory());
}

}  // namespace node_jsbr
