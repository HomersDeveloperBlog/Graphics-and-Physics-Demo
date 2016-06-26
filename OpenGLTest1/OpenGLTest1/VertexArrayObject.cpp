#include "stdafx.h"
#include "VertexArrayObject.h"

OpenGLVertexArrayObject::OpenGLVertexArrayObject()
{
    GLuint glnVAOHandle;
    glGenVertexArrays(1, glnVAOHandle);
    if(GetOpenGLError(__FILE__, __LINE__))
    {
        assert(!glIsVertexArray(glnVAOHandle));
        throw;
    }
        
    m_pglnVAOHandle = shared_ptr<GLint>(
        new GLint(glnVAOHandle), //%could throw. would leak handle.
        [](GLint * i_pglnVAOHandle)
        {
            glDeleteVertexArrays(1, i_pglnVAOHandle);
            assert(!GetOpenGLError(__FILE__, __LINE__));
            
            delete i_pglnVAOHandle;
        });
}
    
void OpenGLVertexArrayObject::Bind() const
{
    glBindVertexArray(*m_pglnVAOHandle);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
}
    
//Lookup is GLint glGetAttribLocation(GLuint program, const GLchar *name);
void OpenGLVertexArrayObject::DefineAttributeSourceFormat(
    int i_nIndex,
    int i_nVectorDimension,
    int i_nStride)
{
    assert(i_nIndex > 0);
    assert(i_nVectorDimension > 0 && i_nStride > 0);
        
    //Bind this VAO
    glBindVertexArray(*m_pglnVAOHandle);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
	
    //Define format
    glVertexAttribPointer(
        static_cast<GLuint>(i_nIndex), //layout number
        static_cast<GLint>(i_nVectorDimension), //as it is packed in the buffer.
        GL_FLOAT, //buffer type (converted from this type to foat)
        GL_FALSE, //normalize?
        static_cast<GLSizei>(i_nStride),
        static_cast<GLvoid *>(nullptr)); //offset from start of bound GL_ARRAY_BUFFER
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
    
    //Enable attribute
    glEnableVertexArrayAttrib(
        *m_pglnVAOHandle,
        i_nIndex);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
 }