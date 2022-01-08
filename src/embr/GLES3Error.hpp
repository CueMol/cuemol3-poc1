#pragma once

namespace embr {

#ifdef MB_DEBUG
// check error - GLES2 version
#define CHK_GLERROR(MSG)                               \
    {                                                  \
        GLenum errc;                                   \
        errc = glGetError();                           \
        if (errc != GL_NO_ERROR)                       \
            MB_DPRINTLN("%s GLError (%d)", MSG, errc); \
        else                                           \
            MB_DPRINTLN("%s noerror", MSG);            \
    }
#else
#define CHK_GLERROR(MSG) glGetError()
#endif

#define CLR_GLERROR() glGetError()

}  // namespace embr
