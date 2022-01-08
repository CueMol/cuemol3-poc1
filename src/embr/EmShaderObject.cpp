// -*-Mode: C++;-*-
//
//  OpenGL ES3 shader object
//

#include <common.h>

#include "EmShaderObject.hpp"

#include <qlib/FileStream.hpp>
#include <qsys/SysConfig.hpp>

#include "GLES3Error.hpp"

namespace embr {

using qsys::SysConfig;

EmShaderObject::~EmShaderObject()
{
    if (m_hGL) {
        MB_DPRINTLN("EmShader %d destroyed", m_hGL);
        glDeleteShader(m_hGL);
    }
}

void EmShaderObject::loadFile(const LString &filename)
{
    CLR_GLERROR();
    // CHK_GLERROR("SO.loadFile createShader BEFORE");

    // m_hGL = glCreateShaderObjectARB(m_nType);
    m_hGL = glCreateShader(m_nType);
    CHK_GLERROR("SO.loadFile createShader");
    GLenum errc;
    errc = glGetError();
    if (errc != GL_NO_ERROR) {
        LOG_DPRINTLN("ShaderObject::ShaderObject(): cannot create shader object: %s",
                     filename.c_str());
        MB_THROW(qlib::RuntimeException, "glCreateShader error");
        return;
    }

    SysConfig *pconf = SysConfig::getInstance();
    LString fnam = pconf->convPathName(filename);

    // read source file
    qlib::FileInStream fis;
    fis.open(fnam);
    char sbuf[1024];
    m_source = "";
    while (fis.ready()) {
        int n = fis.read(sbuf, 0, sizeof sbuf - 1);
        sbuf[n] = '\0';
        m_source += sbuf;
    }

    // set shader source
    const char *s = m_source.c_str();
    int l = m_source.length();

    // glShaderSourceARB( m_hGL, 1, &s, &l );
    glShaderSource(m_hGL, 1, &s, &l);
    if (glGetError() != GL_NO_ERROR) {
        CHK_GLERROR("SO.loadFile");
        LOG_DPRINTLN("ShaderObject::ShaderObject(): cannot set shader source: %s",
                     fnam.c_str());
        MB_THROW(qlib::RuntimeException, "glShaderSource error");
    }

    m_name = fnam;
}

bool EmShaderObject::compile()
{
    int length, l;

    CLR_GLERROR();

    // compile
    glCompileShader(m_hGL);

    // check errors
    GLint result;
    glGetShaderiv(m_hGL, GL_COMPILE_STATUS, &result);

    if (glGetError() != GL_NO_ERROR || result == GL_FALSE) {
        LOG_DPRINTLN("ShaderObject::Compile(): cannot compile shader: %s",
                     m_name.c_str());
        glGetShaderiv(m_hGL, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            GLchar *info_log = new GLchar[length];
            // glGetInfoLogARB( m_hGL, length, &l, info_log );
            glGetShaderInfoLog(m_hGL, length, &l, info_log);
            LOG_DPRINTLN("%s", info_log);
            delete[] info_log;
        }
        MB_THROW(qlib::RuntimeException, "glCompileSource error");
        return false;
    } else {
#ifdef MB_DEBUG
        glGetShaderiv(m_hGL, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            GLchar *info_log = new GLchar[length];
            glGetShaderInfoLog(m_hGL, length, &l, info_log);
            LOG_DPRINTLN("EmSO> %s", info_log);
            delete[] info_log;
        }
#endif
    }

    return true;
}

}  // namespace embr
