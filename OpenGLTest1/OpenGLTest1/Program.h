#pragma once

#include <string>
#include <memory>

#include "GLBase.h"

class OpenGLShader;

class OpenGLProgram
{
public:
    OpenGLProgram();
    
    GLint RawHandle() const {return *m_pglnProgramHandle;}
    
    void AttachShader(const OpenGLShader & i_oShader);
    void DetachShader(const OpenGLShader & i_oShader);
    bool GetLinkStatus() const;
    bool Link();
    std::string GetLinkerMessage() const;
    void Use() const;
    void DetachAll();
	GLuint GetAttributeLocation(const std::string & i_strName);

private:
    std::shared_ptr<GLint> m_pglnProgramHandle; //%actually a GLuint, is a GLint on return to check correctness only.
};