// -*-Mode: C++;-*-
//
//  GLSL sphere rendering helper class
//

#pragma once

#include <gfx/ProgramObject.hpp>
#include <qsys/EsDisplayContext.hpp>

namespace molvis {

class GLSLSphereHelper
{
private:
    struct SphElem
    {
        qfloat32 cenx, ceny, cenz;
        qfloat32 dspx, dspy;
        qfloat32 rad;
        qbyte r, g, b, a;
    };

    // typedef gfx::DrawAttrElems<quint16, SphElem> SphElemAry16;
    using SphElemAry32 = gfx::DrawAttrElems<quint32, SphElem>;

    // Attribute location ID
    // These values should coincide with the location layout qualifiers in the shader
    static const int ATTR_VERTEX_LOC = 0;
    static const int ATTR_IMPOS_LOC = 1;
    static const int ATTR_RAD_LOC = 2;
    static const int ATTR_COL_LOC = 3;

    gfx::ProgramObject *m_pPO;

    SphElemAry32 *m_pDrawElem;

    qfloat32 dsps[4][2];

public:
    GLSLSphereHelper() : m_pPO(NULL), m_pDrawElem(NULL)
    {
        dsps[0][0] = -1.0f;
        dsps[0][1] = -1.0f;
        dsps[1][0] = 1.0f, dsps[1][1] = -1.0f;
        dsps[2][0] = -1.0f, dsps[2][1] = 1.0f;
        dsps[3][0] = 1.0f, dsps[3][1] = 1.0f;
    }

    ~GLSLSphereHelper()
    {
        invalidate();
    }

    bool initShader(qsys::Renderer *pRend, qsys::EsDisplayContext *pCtxt)
    {
        MB_ASSERT(m_pPO == NULL);
        // sysdep::ShaderSetupHelper<qsys::Renderer> ssh(pRend);

        // if (!ssh.checkEnvVS()) {
        //     MB_DPRINTLN("GLShader not supported");
        //     return false;
        // }

        if (m_pPO == NULL) {
            m_pPO = pCtxt->createProgramObject("gpu_sphere");
            qlib::MapTable<qlib::LString> file_names;
            file_names.set("vertex", "%%CONFDIR%%/shaders/sphere_vertex.glsl");
            file_names.set("fragment", "%%CONFDIR%%/shaders/sphere_frag.glsl");
            m_pPO->loadShaders(file_names);
        }
        if (m_pPO == NULL) {
            LOG_DPRINTLN("GPUSphere> ERROR: cannot create progobj.");
            return false;
        }

        // setup attributes
        // m_nVertexLoc = m_pPO->getAttribLocation("a_vertex");
        // m_nImposLoc = m_pPO->getAttribLocation("a_impos");
        // m_nRadLoc = m_pPO->getAttribLocation("a_radius");
        // m_nColLoc = m_pPO->getAttribLocation("a_color");

        return true;
    }

    void alloc(int nsphs)
    {
        SphElemAry32 *pdata = MB_NEW SphElemAry32();
        m_pDrawElem = pdata;
        SphElemAry32 &sphdata = *pdata;
        sphdata.setAttrSize(4);
        sphdata.setAttrInfo(0, ATTR_VERTEX_LOC, 3, qlib::type_consts::QTC_FLOAT32,
                            offsetof(SphElem, cenx));
        sphdata.setAttrInfo(1, ATTR_IMPOS_LOC, 2, qlib::type_consts::QTC_FLOAT32,
                            offsetof(SphElem, dspx));
        sphdata.setAttrInfo(2, ATTR_RAD_LOC, 1, qlib::type_consts::QTC_FLOAT32,
                            offsetof(SphElem, rad));
        sphdata.setAttrInfo(3, ATTR_COL_LOC, 4, qlib::type_consts::QTC_UINT8,
                            offsetof(SphElem, r));

        sphdata.alloc(nsphs * 4);
        sphdata.allocInd(nsphs * 6);
        sphdata.setDrawMode(gfx::AbstDrawElem::DRAW_TRIANGLES);
    }

    void setData(int ind, const Vector4D &pos, double rad, ColorPtr pc,
                 qlib::uid_t nSceneID = qlib::invalid_uid)
    {
        // qfloat32 dsps[4][2] = ;

        int i = ind * 4;
        int ifc = ind * 6;

        SphElemAry32 &sphdata = *m_pDrawElem;
        SphElem data;

        data.cenx = (qfloat32)pos.x();
        data.ceny = (qfloat32)pos.y();
        data.cenz = (qfloat32)pos.z();
        data.rad = (qfloat32)rad;

        quint32 devcode = pc->getDevCode(nSceneID);
        data.r = (qbyte)gfx::getRCode(devcode);
        data.g = (qbyte)gfx::getGCode(devcode);
        data.b = (qbyte)gfx::getBCode(devcode);
        data.a = (qbyte)gfx::getACode(devcode);

        sphdata.atind(ifc) = i + 0;
        ++ifc;
        sphdata.atind(ifc) = i + 1;
        ++ifc;
        sphdata.atind(ifc) = i + 2;
        ++ifc;
        sphdata.atind(ifc) = i + 2;
        ++ifc;
        sphdata.atind(ifc) = i + 1;
        ++ifc;
        sphdata.atind(ifc) = i + 3;
        ++ifc;

        for (int j = 0; j < 4; ++j) {
            sphdata.at(i) = data;
            sphdata.at(i).dspx = dsps[j][0];
            sphdata.at(i).dspy = dsps[j][1];
            ++i;
        }
    }

    gfx::AbstDrawElem *getDrawElem() const
    {
        return m_pDrawElem;
    }

    void draw(DisplayContext *pdc, qlib::uid_t nSceneID = qlib::invalid_uid)
    {
        if (m_pDrawElem == NULL)
            // ERROR??
            return;

        m_pPO->enable();

        // TODO: alpha/edge support using uniform buffer
        // m_pPO->setUniformF("frag_alpha", pdc->getAlpha());
        // if (pdc->getEdgeLineType() != DisplayContext::ELT_NONE) {
        //     m_pPO->setUniformF("u_edge", pdc->getEdgeLineWidth());
        //     float r = .0f, g = .0f, b = .0f;
        //     ColorPtr pcol = pdc->getEdgeLineColor();
        //     if (!pcol.isnull()) {
        //         quint32 dcc = pcol->getDevCode(nSceneID);
        //         r = gfx::convI2F(gfx::getRCode(dcc));
        //         g = gfx::convI2F(gfx::getGCode(dcc));
        //         b = gfx::convI2F(gfx::getBCode(dcc));
        //     }
        //     m_pPO->setUniformF("u_edgecolor", r, g, b, 1);
        //     if (pdc->getEdgeLineType() == DisplayContext::ELT_SILHOUETTE)
        //         m_pPO->setUniform("u_bsilh", 1);
        //     else
        //         m_pPO->setUniform("u_bsilh", 0);
        // } else {
        //     m_pPO->setUniformF("u_edge", 0.0);
        //     m_pPO->setUniformF("u_edgecolor", 0, 0, 0, 1);
        //     m_pPO->setUniform("u_bsilh", 0);
        // }

        pdc->drawElem(*m_pDrawElem);
        m_pPO->disable();
    }

    void invalidate()
    {
        if (m_pDrawElem != NULL) {
            delete m_pDrawElem;
            m_pDrawElem = NULL;
        }
    }
};

}  // namespace molvis
