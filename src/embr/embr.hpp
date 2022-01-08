#pragma once

#ifdef __APPLE__
#include <OpenGLES/ES3/gl.h>
#else
// #include <EGL/egl.h>
// #include <EGL/eglext.h>
#include <GLES3/gl3.h>
#endif

namespace embr {

bool init();
void fini();

}  // namespace embr
