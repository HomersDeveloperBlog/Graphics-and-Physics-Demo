#include "stdafx.h"
#include "assert.h"

#include "Buffer.h"

using namespace std;

OpenGLBuffer::OpenGLBuffer(
    size_t i_nSize, //In bytes
    const void * i_pData)
{
    //Create buffer object
    GLuint glnBufferHandle = -1;
    glGenBuffers(
		static_cast<GLsizei>(1),
		&glnBufferHandle);
    if(GetOpenGLError(__FILE__, __LINE__))
    {
        assert(!glIsBuffer(glnBufferHandle));
        throw;
    }
    
    m_pglnBufferHandle = shared_ptr<GLuint>(
        new GLuint(glnBufferHandle), //%could throw. would leak handle.
        [](GLuint * i_pglnBufferHandle)
        {
            glDeleteBuffers(
				static_cast<GLsizei>(1),
				i_pglnBufferHandle);
            assert(!GetOpenGLError(__FILE__, __LINE__));
            
            delete i_pglnBufferHandle;
        });
        
    //Initial binding
    glBindBuffer(
        GL_ARRAY_BUFFER, //%in need of abstracting.
        *m_pglnBufferHandle);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
        
    //Fill buffer
    glNamedBufferData( //Overwrites the whole buffer.
        *m_pglnBufferHandle, 
        static_cast<GLsizei>(i_nSize), //%need to verify integer conversion. 
        i_pData, 
        GL_STATIC_DRAW); //%needs abstracting
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
}
    
void OpenGLBuffer::Bind() const
{
    glBindBuffer(
        GL_ARRAY_BUFFER, //%in need of abstracting.
        *m_pglnBufferHandle);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
}

size_t OpenGLBuffer::Size() const
{
    GLint nAPIReturn = -1;
    glGetNamedBufferParameteriv(
        *m_pglnBufferHandle,
        GL_BUFFER_SIZE,
        &nAPIReturn);
    assert(nAPIReturn >= 0);
    if(GetOpenGLError(__FILE__, __LINE__)
        || nAPIReturn < 0)
        throw;
    
    return static_cast<size_t>(nAPIReturn);
}