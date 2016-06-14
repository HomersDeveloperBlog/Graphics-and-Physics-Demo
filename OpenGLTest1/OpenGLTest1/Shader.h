#pragma once

#include <string>
#include <memory>

#include "GL\glew.h" //%only typedefs are needed
#include "GL\wglew.h"
#include "GL\freeglut.h"

class OpenGLShader
{
    OpenGLShader(const OpenGLShaderSource & i_oShaderSource);
    
    GLint RawHandle() const {return m_pglnShaderHandle;}
    
    bool GetCompileStatus() const;
    bool Compile();
    string GetCompilerMessage() const;
    
private:
    shared_ptr<GLint> m_pglnShaderHandle;
};