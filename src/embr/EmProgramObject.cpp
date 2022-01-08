#include "EmProgramObject.hpp"

#include <qlib/FileStream.hpp>

#include "EmView.hpp"
#include "GLES3Error.hpp"

namespace embr {

bool EmProgramObject::init()
{
    GLenum errc;

    CLR_GLERROR();

    m_hPO = glCreateProgram();
    errc = glGetError();

    if (errc != GL_NO_ERROR) {
        // LOG_DPRINTLN("ProgramObject::ProgramObject(): cannot create program object
        // (%d; %s)", errc, gluErrorString(errc));
        return false;
    }

    return true;
}

EmProgramObject::~EmProgramObject()
{
    MB_DPRINTLN("EmProgramObj %d destroyed", m_hPO);
    clear();
    glDeleteProgram(m_hPO);
}

void EmProgramObject::clear()
{
    for (ShaderTab::value_type &elem : m_shaders) {
        delete elem.second;
    }
    m_shaders.clear();
}

bool EmProgramObject::loadShader(const LString &name, const LString &srcpath,
                                 GLenum shader_type)
{
    ShaderTab::const_iterator i = m_shaders.find(name);
    if (i != m_shaders.end()) return false;

    EmShaderObject *pVS = new EmShaderObject(shader_type);
    if (pVS == NULL) return false;

    LOG_DPRINTLN("PO> Loading shader: %s", srcpath.c_str());
    pVS->loadFile(srcpath);
    pVS->compile();
    attach(pVS);
    m_shaders.insert(ShaderTab::value_type(name, pVS));

    LOG_DPRINTLN("PO> Loading shader OK");
    return true;
}

void EmProgramObject::attach(const EmShaderObject *s)
{
    CLR_GLERROR();

    glAttachShader(m_hPO, s->getHandle());

    if (glGetError() != GL_NO_ERROR) {
        MB_THROW(qlib::RuntimeException, "glAttachShader error");
    }
}

bool EmProgramObject::link()
{
    int length, l;

    CLR_GLERROR();

    // link
    glLinkProgram(m_hPO);

    // get errors
    GLint result;
    glGetProgramiv(m_hPO, GL_LINK_STATUS, &result);

    if (glGetError() != GL_NO_ERROR || result == GL_FALSE) {
        LOG_DPRINTLN("ProgramObject.link(): cannot link program object");

        glGetProgramiv(m_hPO, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            GLchar *info_log = new GLchar[length];
            glGetProgramInfoLog(m_hPO, length, &l, info_log);
            LOG_DPRINTLN("%s", info_log);
            delete[] info_log;
        }
        MB_THROW(qlib::RuntimeException, "glLinkProgram error");
        // return false;
    } else {
#ifdef MB_DEBUG
        glGetProgramiv(m_hPO, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            GLchar *info_log = new GLchar[length];
            glGetProgramInfoLog(m_hPO, length, &l, info_log);
            LOG_DPRINTLN("EmPO> %s", info_log);
            delete[] info_log;
        }
#endif
    }

    return true;
}

void EmProgramObject::use()
{
    CLR_GLERROR();

    glUseProgram(m_hPO);
    CHK_GLERROR("PO.use");
}

void EmProgramObject::validate()
{
    GLint logLength, status;

    glValidateProgram(m_hPO);
    glGetProgramiv(m_hPO, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = new GLchar[logLength];
        glGetProgramInfoLog(m_hPO, logLength, &logLength, log);
        // NSLog(@"Program validate log:\n%s", log);
        LOG_DPRINTLN("EmPO validate> %s", log);
        delete[] log;
    }

    glGetProgramiv(m_hPO, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        // NSLog(@"Failed to validate program %d", prog);
        LOG_DPRINTLN("EmPO validate> FAILED!!");
    }

    return;
}

void EmProgramObject::setProgParam(GLenum pname, GLint param)
{
    CLR_GLERROR();
    glProgramParameteri(m_hPO, pname, param);
    CHK_GLERROR("PO.setProgParam");
}

void EmProgramObject::setMatrix(const char *name, const qlib::Matrix4D &mat)
{
    GLfloat m[16];

    m[0] = mat.aij(1, 1);
    m[1] = mat.aij(1, 2);
    m[2] = mat.aij(1, 3);
    m[3] = mat.aij(1, 4);

    m[4] = mat.aij(2, 1);
    m[5] = mat.aij(2, 2);
    m[6] = mat.aij(2, 3);
    m[7] = mat.aij(2, 4);

    m[8] = mat.aij(3, 1);
    m[9] = mat.aij(3, 2);
    m[10] = mat.aij(3, 3);
    m[11] = mat.aij(3, 4);

    m[12] = mat.aij(4, 1);
    m[13] = mat.aij(4, 2);
    m[14] = mat.aij(4, 3);
    m[15] = mat.aij(4, 4);

    setMatrix4fv(name, 1, GL_FALSE, m);
}

}  // namespace embr
