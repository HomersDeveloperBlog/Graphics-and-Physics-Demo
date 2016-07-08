#pragma once

#include <memory>

#include "GLBase.h"

class OpenGLVertexArrayObject
{
public:
    OpenGLVertexArrayObject();
    
    GLuint RawHandle() const {return *m_pglnVAOHandle;}
    
    void Bind() const;
    void DefineAttributeSourceFormat(
        int i_nIndex,
        int i_nVectorDimension,
        int i_nStride);
    
private:
    std::shared_ptr<GLuint> m_pglnVAOHandle;
};


