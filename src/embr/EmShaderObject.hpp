#pragma once

#include <qlib/LString.hpp>

#include "embr.hpp"

namespace embr {

class EmView;

using qlib::LString;

class EmShaderObject
{
private:
    GLuint m_hGL;
    LString m_name;
    GLenum m_nType;
    LString m_source;

public:
    EmShaderObject(const GLenum shader_type) : m_nType(shader_type), m_hGL(0) {}

    virtual ~EmShaderObject();

    void loadFile(const LString &filename);

    bool compile();

    inline GLuint getHandle() const
    {
        return m_hGL;
    }
};
}  // namespace embr
