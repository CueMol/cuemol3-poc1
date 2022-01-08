#pragma once

#include <gfx/DrawAttrArray.hpp>
#include "EmView.hpp"

namespace embr {

int convGLConsts(int id)
{
    switch (id) {
        case qlib::type_consts::QTC_BOOL:
            return GL_BOOL;
        case qlib::type_consts::QTC_UINT8:
            return GL_UNSIGNED_BYTE;
        case qlib::type_consts::QTC_UINT16:
            return GL_UNSIGNED_SHORT;
        case qlib::type_consts::QTC_UINT32:
            return GL_UNSIGNED_INT;
        case qlib::type_consts::QTC_INT8:
            return GL_BYTE;
        case qlib::type_consts::QTC_INT16:
            return GL_SHORT;
        case qlib::type_consts::QTC_INT32:
            return GL_INT;
        case qlib::type_consts::QTC_FLOAT32:
            return GL_FLOAT;
        case qlib::type_consts::QTC_FLOAT64:
            LOG_DPRINTLN("GL_DOUBLE not supported!!");
            // return GL_DOUBLE;
        default:
            return -1;
    }
}

int convGLNorm(int id)
{
    if (id == qlib::type_consts::QTC_FLOAT32 || id == qlib::type_consts::QTC_FLOAT64)
        return GL_FALSE;
    else
        return GL_TRUE;
}

using gfx::DrawElem;

GLenum convDrawMode(int nMode)
{
    GLenum mode;
    switch (nMode) {
        case DrawElem::DRAW_POINTS:
            mode = GL_POINTS;
            break;
        case DrawElem::DRAW_LINE_STRIP:
            mode = GL_LINE_STRIP;
            break;
        case DrawElem::DRAW_LINE_LOOP:
            mode = GL_LINE_LOOP;
            break;
        case DrawElem::DRAW_LINES:
            mode = GL_LINES;
            break;
        case DrawElem::DRAW_TRIANGLE_STRIP:
            mode = GL_TRIANGLE_STRIP;
            break;
        case DrawElem::DRAW_TRIANGLE_FAN:
            mode = GL_TRIANGLE_FAN;
            break;
        case DrawElem::DRAW_TRIANGLES:
            mode = GL_TRIANGLES;
            break;
        // case DrawElem::DRAW_QUAD_STRIP:
        //     mode = GL_QUAD_STRIP;
        //     break;
        // case DrawElem::DRAW_QUADS:
        //     mode = GL_QUADS;
        //     break;
        // case DrawElem::DRAW_POLYGON:
        //     mode = GL_POLYGON;
        //     break;
        default: {
            LString msg = "Ogl DrawElem: invalid draw mode";
            LOG_DPRINTLN(msg);
            MB_THROW(qlib::RuntimeException, msg);
        }
    }
    return mode;
}

class EmVBORep : public gfx::VBORep
{
public:
    qlib::uid_t m_nViewID;
    GLuint m_nBufID;
    GLuint m_nVAO;
    bool m_bDataInitDone;

    EmVBORep(EmView *pView, const gfx::AbstDrawAttrs &data)
        : m_nViewID(pView->getUID()), m_bDataInitDone(false)
    {
        glGenBuffers(1, &m_nBufID);
        glGenVertexArrays(1, &m_nVAO);

        // setup VAO
        glBindVertexArray(m_nVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_nBufID);
        size_t nattr = data.getAttrSize();
        for (size_t i = 0; i < nattr; ++i) {
            int al = data.getAttrLoc(i);
            int az = data.getAttrElemSize(i);
            int at = data.getAttrTypeID(i);
            int ap = data.getAttrPos(i);
            glEnableVertexAttribArray(al);
            glVertexAttribPointer(al, az, convGLConsts(at), convGLNorm(at),
                                  data.getElemSize(), (void *)ap);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        data.setVBO(this);
    }

    void drawBuffer(const gfx::AbstDrawAttrs &data)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_nBufID);
        if (!m_bDataInitDone) {
            // Init VBO & copy data
            glBufferData(GL_ARRAY_BUFFER, data.getDataSize(), data.getData(),
                         GL_STATIC_DRAW);
        } else {
            if (data.isUpdated()) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, data.getDataSize(), data.getData());
            }
        }

        GLenum mode = convDrawMode(data.getDrawMode());

        glBindVertexArray(m_nVAO);
        glDrawArrays(mode, 0, data.getSize());
        glBindVertexArray(0);
    }

    virtual ~EmVBORep()
    {
        qsys::ViewPtr pView = qsys::SceneManager::getViewS(m_nViewID);
        if (pView.isnull()) {
            // If any views aren't found, it is no problem,
            // because the parent context (and also all DLs) may be already destructed.
            return;
        }
        gfx::DisplayContext *pctxt = pView->getDisplayContext();
        pctxt->setCurrent();

        GLuint buffers[1];
        buffers[0] = m_nBufID;
        glDeleteBuffers(1, buffers);
    }
};


}  // namespace embr
