#pragma once

#include <string>
#include <memory>

#include "GL\glew.h" //%only typedefs are needed
#include "GL\wglew.h"
#include "GL\freeglut.h"

#include "GLSLBuild.h"

class OpenGLShader
{
public:
    OpenGLShader(const OpenGLShaderSource & i_oShaderSource);
    
    GLint RawHandle() const {return *m_pglnShaderHandle;}
    
    bool GetCompileStatus() const;
    bool Compile();
    std::string GetCompilerMessage() const;
    
private:
    std::shared_ptr<GLint> m_pglnShaderHandle;
};