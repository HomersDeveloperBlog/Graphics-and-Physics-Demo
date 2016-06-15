#pragma once

#include <string>
#include <memory>

#include "GL\glew.h" //%only typedefs are needed
#include "GL\wglew.h"
#include "GL\freeglut.h"

#include "Shader.h"

class OpenGLProgram
{
    OpenGLProgram();
    
    GLint RawHandle() const {return *m_pglnProgramHandle;}
    
    void AttachShader(const OpenGLShader & i_oShader);
    void DetachShader(const OpenGLShader & i_oShader);
    bool GetLinkStatus() const;
    bool Link();
    string GetLinkerMessage() const;
    void Use() const;
    void DetachAll();
    
private:
    shared_ptr<GLint> m_pglnProgramHandle;
};