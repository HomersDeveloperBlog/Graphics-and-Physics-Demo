OpenGLBuffer::OpenGLBuffer(
    size_t i_nSize, //In bytes
    const void * i_pData)
{
    //Create buffer object
    GLuint glnBufferHandle;
    glGenBuffers(1, glnBufferHandle);
    if(GetOpenGLError(__FILE__, __LINE__))
    {
        assert(!glIsBuffer(glnBufferHandle));
        throw;
    }
    
    m_pglnBufferHandle = shared_ptr<GLint>(
        new GLint(glnBufferHandle), //%could throw. would leak handle.
        [](GLint * i_pglnBufferHandle)
        {
            glDeleteBuffers(1, i_pglnBufferHandle);
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
