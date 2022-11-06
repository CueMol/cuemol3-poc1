// -*-Mode: C++;-*-
//
//  CPK molecular renderer class (version 2)
//

#include <common.h>

#include "CPK2Renderer.hpp"

#include <gfx/DrawAttrArray.hpp>
#include <gfx/SphereSet.hpp>
#include <modules/molstr/AtomIterator.hpp>
#include <qsys/EsDisplayList.hpp>
#include <qsys/Scene.hpp>
#include <qsys/View.hpp>

#include "molvis.hpp"

// #include "GLSLSphereHelper.hpp"

using namespace molvis;
using namespace molstr;

class MyMesh : public qsys::TrigMesh
{
public:
    using super_t = qsys::TrigMesh;
    using index_t = qlib::quint32;

    bool color(int ind, qlib::quint32 cc)
    {
        if (ind < 0 || getSize() <= ind) return false;

        // uint8 color
        // super_t::at(ind).r = gfx::getRCode(cc);
        // super_t::at(ind).g = gfx::getGCode(cc);
        // super_t::at(ind).b = gfx::getBCode(cc);
        // super_t::at(ind).a = gfx::getACode(cc);

        // float32 color
        super_t::at(ind).r = gfx::getFR(cc);
        super_t::at(ind).g = gfx::getFG(cc);
        super_t::at(ind).b = gfx::getFB(cc);
        super_t::at(ind).a = gfx::getFA(cc);

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
        printf("normal %d (%f, %f, %f)\n", ind, super_t::at(ind).nx,
               super_t::at(ind).ny, super_t::at(ind).nz);
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

    void startIndexTriangles(int nverts, int nfaces)
    {
        super_t::setDrawMode(super_t::DRAW_TRIANGLES);

        super_t::setAttrSize(3);
        super_t::setAttrInfo(0, qsys::EsDisplayList::DSLOC_VERT_POS, 4,
                             qlib::type_consts::QTC_FLOAT32,
                             offsetof(qsys::TrigVertAttr, x));
        super_t::setAttrInfo(1, qsys::EsDisplayList::DSLOC_VERT_COLOR, 4,
                             qlib::type_consts::QTC_FLOAT32,
                             offsetof(qsys::TrigVertAttr, r));
        super_t::setAttrInfo(2, qsys::EsDisplayList::DSLOC_VERT_NORMAL, 4,
                             qlib::type_consts::QTC_FLOAT32,
                             offsetof(qsys::TrigVertAttr, nx));

        super_t::alloc(nverts);
        allocInd(nfaces * 3);
    }
};

template <typename _DrawElemType>
class VBOSphereSetTrait
{
private:
    struct Sphere
    {
        Vector4D posr;
        quint32 ccode;
    };

    typedef std::vector<Sphere> datatype;

    typedef gfx::SphereSetTmpl<VBOSphereSetTrait> outer_t;

    datatype m_data;

    /// default alpha (multiplied to all alpha comp)
    double m_defAlpha;

    /// tesselation detail (common to all spheres)
    int m_nDetail;

public:
    VBOSphereSetTrait() : m_defAlpha(1.0), m_nDetail(10) {}

    ~VBOSphereSetTrait() {}

    void setAlpha(double d)
    {
        m_defAlpha = d;
    }

    void create(int nsize, int ndetail)
    {
        m_nDetail = ndetail;
        m_data.resize(nsize);
    }

    void sphere(int index, const Vector4D &pos, double r, const ColorPtr &col)
    {
        m_data[index].posr = pos;
        m_data[index].posr.w() = r;
        if (qlib::isNear4(m_defAlpha, 1.0)) {
            m_data[index].ccode = col->getCode();
        } else {
            m_data[index].ccode = gfx::mixAlpha(col->getCode(), m_defAlpha);
        }
    }

    const Vector4D &getPos(int isph) const
    {
        return m_data[isph].posr;
    }
    double getRadius(int isph) const
    {
        return m_data[isph].posr.w();
    }
    int getDetail(int isph) const
    {
        return m_nDetail;
    }

    /////////////////////////////

    _DrawElemType *m_pVary;

    void color(quint32 isph, quint32 ivert)
    {
        quint32 col = m_data[isph].ccode;
        m_pVary->color(ivert, col);
    }

    void normal(quint32 ind, const Vector4D &v)
    {
        m_pVary->normal(ind, v);
    }

    void vertex(quint32 ind, const Vector4D &v)
    {
        m_pVary->vertex(ind, v);
    }

    void face(quint32 ifc, quint32 n1, quint32 n2, quint32 n3)
    {
        m_pVary->setIndex3(ifc, n1, n2, n3);
    }

    Vector4D getVertex(quint32 ind) const
    {
        Vector4D rv;
        m_pVary->getVertex(ind, rv);
        return rv;
    }

    /// build draw elem objects
    _DrawElemType *buildDrawElem(outer_t *pOuter)
    {
        int nverts, nfaces;
        pOuter->estimateMeshSize(m_nDetail, nverts, nfaces);
        int nsphs = m_data.size();

        int nvtot = nverts * nsphs;
        int nftot = nfaces * nsphs;
        MB_DPRINTLN("Sph> nv_tot = %d, nf_fot = %d", nvtot, nftot);

        // Create DrawElemVNCI (or VNI?) object
        m_pVary = MB_NEW _DrawElemType();
        m_pVary->startIndexTriangles(nvtot, nftot);
        // m_pVary->setDrawMode(gfx::AbstDrawAttrs::DRAW_TRIANGLES);
        // m_pVary->alloc(nverts);
        // m_pVary->allocInd(nfaces * 3);

        int ivt = 0, ifc = 0;
        for (int i = 0; i < nsphs; ++i) {
            pOuter->buildSphere(i, ivt, ifc);
        }

        return m_pVary;
    }
};

CPK2Renderer::CPK2Renderer()
{
    m_pDrawElem = NULL;
    m_bUseShader = false;
    // m_bCheckShaderOK = false;
    m_bCheckShaderOK = true;
    m_nGlRendMode = REND_DEFAULT;

    // m_pSlSph = MB_NEW GLSLSphereHelper();
}

CPK2Renderer::~CPK2Renderer()
{
    // delete m_pSlSph;
}

const char *CPK2Renderer::getTypeName() const
{
    return "cpk";
}

/////////

void CPK2Renderer::display(DisplayContext *pdc)
{
    if (pdc->isFile()) {
        // case of the file (non-ogl) rendering
        // always use the old version.
        super_t::display(pdc);
        return;
    }

    // VBO rendering mode
    if (m_pDrawElem == NULL) {
        renderVBOImpl();
        if (m_pDrawElem == NULL) return;  // Error, Cannot draw anything (ignore)
    }

    preRender(pdc);
    pdc->setLighting(true);
    pdc->drawElem(*m_pDrawElem);
    postRender(pdc);

    // if (!m_bCheckShaderOK) {
    //     if (m_pSlSph->initShader(this)) {
    //         MB_DPRINTLN("CPK2 sphere shader OK");
    //         m_bUseShader = true;
    //     } else {
    //         m_bUseShader = false;
    //     }
    //     m_bCheckShaderOK = true;
    // }

    // if (m_bUseShader &&
    //     (m_nGlRendMode == REND_DEFAULT || m_nGlRendMode == REND_SHADER)) {
    //     // shader rendering mode
    //     if (m_pSlSph->getDrawElem() == NULL) {
    //         renderShaderImpl();
    //         if (m_pSlSph->getDrawElem() == NULL)
    //             return;  // Error, Cannot draw anything (ignore)
    //     }

    //     preRender(pdc);
    //     m_pSlSph->draw(pdc);
    //     postRender(pdc);
    // } else if (pdc->isDrawElemSupported() &&
    //            (m_nGlRendMode == REND_DEFAULT || m_nGlRendMode == REND_VBO)) {
    //     // VBO rendering mode
    //     if (m_pDrawElem == NULL) {
    //         renderVBOImpl();
    //         if (m_pDrawElem == NULL) return;  // Error, Cannot draw anything (ignore)
    //     }

    //     preRender(pdc);
    //     pdc->drawElem(*m_pDrawElem);
    //     postRender(pdc);
    // } else {
    //     // old version (uses DisplayContext::sphere)
    //     super_t::display(pdc);
    // }
}

void CPK2Renderer::invalidateDisplayCache()
{
    super_t::invalidateDisplayCache();

    if (m_pDrawElem != NULL) {
        delete m_pDrawElem;
        m_pDrawElem = NULL;
    }
    // if (m_bUseShader) {
    //     m_pSlSph->invalidate();
    // }
}

void CPK2Renderer::unloading()
{
    if (m_pDrawElem != NULL) {
        delete m_pDrawElem;
        m_pDrawElem = NULL;
    }
    // if (m_bUseShader) {
    //     m_pSlSph->invalidate();
    // }

    super_t::unloading();
}

double CPK2Renderer::getVdWRadius(MolAtomPtr pAtom)
{
    switch (pAtom->getElement()) {
        case ElemSym::H:
            return m_vdwr_H;

        case ElemSym::C:
            return m_vdwr_C;

        case ElemSym::N:
            return m_vdwr_N;

        case ElemSym::O:
            return m_vdwr_O;

        case ElemSym::S:
            return m_vdwr_S;

        case ElemSym::P:
            return m_vdwr_P;

        default:
            return m_vdwr_X;
    }
}

void CPK2Renderer::propChanged(qlib::LPropEvent &ev)
{
    if (ev.getName().equals("detail")) {
        invalidateDisplayCache();
    } else if (ev.getName().startsWith("vdwr_")) {
        invalidateDisplayCache();
    } else if (ev.getParentName().equals("coloring") ||
               ev.getParentName().startsWith("coloring.")) {
        invalidateDisplayCache();
    }

    MolAtomRenderer::propChanged(ev);
}

/////////

bool CPK2Renderer::isRendBond() const
{
    return false;
}

void CPK2Renderer::rendBond(DisplayContext *pdl, MolAtomPtr pAtom1, MolAtomPtr pAtom2,
                            MolBond *pMB)
{
}

void CPK2Renderer::beginRend(DisplayContext *pdl)
{
    // m_nDetailOld = pdl->getDetail();
    // setupDetail(pdl, m_nDetail);
}

void CPK2Renderer::endRend(DisplayContext *pdl)
{
    // pdl->setDetail(m_nDetailOld);
}

void CPK2Renderer::rendAtom(DisplayContext *pdl, MolAtomPtr pAtom, bool)
{
    // pdl->color(ColSchmHolder::getColor(pAtom));
    // pdl->sphere(getVdWRadius(pAtom), pAtom->getPos());
}

/////////////////////
// VBO implementation

void CPK2Renderer::renderVBOImpl()
{
    MolCoordPtr pMol = getClientMol();
    if (pMol.isnull()) {
        MB_DPRINTLN("CPK2Renderer::render> Client mol is null");
        return;
    }

    // estimate the size of drawing elements
    int nsphs = 0;
    {
        AtomIterator iter(pMol, getSelection());
        for (iter.first(); iter.hasMore(); iter.next()) {
            int aid = iter.getID();
            MolAtomPtr pAtom = pMol->getAtom(aid);
            if (pAtom.isnull()) continue;  // ignore errors
            ++nsphs;
        }
    }

    if (nsphs == 0) return;  // nothing to draw

    // initialize the coloring scheme
    getColSchm()->start(pMol, this);
    pMol->getColSchm()->start(pMol, this);

    gfx::SphereSetTmpl<VBOSphereSetTrait<MyMesh>> sphs2;

    sphs2.getdata().create(nsphs, m_nDetail);
    if (!useShaderAlpha()) {
        sphs2.getdata().setAlpha(getDefaultAlpha());
    }

    // build meshes / DrawAttr
    {
        AtomIterator iter(pMol, getSelection());
        int i = 0;
        for (iter.first(); iter.hasMore(); iter.next()) {
            int aid = iter.getID();
            MolAtomPtr pAtom = pMol->getAtom(aid);
            if (pAtom.isnull()) {
                // ignore errors
                continue;
            }
            sphs2.getdata().sphere(i, pAtom->getPos(), getVdWRadius(pAtom),
                                   ColSchmHolder::getColor(pAtom));
            ++i;
        }
    }

    m_pDrawElem = sphs2.getdata().buildDrawElem(&sphs2);
    m_pDrawElem->setUpdated(true);
    // finalize the coloring scheme
    getColSchm()->end();
    pMol->getColSchm()->end();
}

//////////////////////
// GLSL implementation

// void CPK2Renderer::renderShaderImpl()
// {
//     MolCoordPtr pMol = getClientMol();
//     if (pMol.isnull()) {
//         MB_DPRINTLN("CPK2Renderer::render> Client mol is null");
//         return;
//     }

//     // estimate the size of drawing elements
//     int nsphs = 0;
//     {
//         AtomIterator iter(pMol, getSelection());
//         for (iter.first(); iter.hasMore(); iter.next()) {
//             int aid = iter.getID();
//             MolAtomPtr pAtom = pMol->getAtom(aid);
//             if (pAtom.isnull()) continue;  // ignore errors
//             ++nsphs;
//         }
//     }

//     if (nsphs == 0) return;  // nothing to draw

//     // initialize the coloring scheme
//     getColSchm()->start(pMol, this);
//     pMol->getColSchm()->start(pMol, this);

//     m_pSlSph->alloc(nsphs);

//     {
//         AtomIterator iter(pMol, getSelection());
//         int i = 0;  //, j, ifc=0;
//         Vector4D pos;
//         for (iter.first(); iter.hasMore(); iter.next()) {
//             int aid = iter.getID();
//             MolAtomPtr pAtom = pMol->getAtom(aid);
//             if (pAtom.isnull()) continue;  // ignore errors

//             m_pSlSph->setData(i, pAtom->getPos(), getVdWRadius(pAtom),
//                               ColSchmHolder::getColor(pAtom), getSceneID());
//             ++i;
//         }
//     }

//     // finalize the coloring scheme
//     getColSchm()->end();
//     pMol->getColSchm()->end();
// }
