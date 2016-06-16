#pragma once

class OpenGLBuffer
{
public:
    OpenGLBuffer(
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
    
    GLint RawHandle() const 
    {
        return *m_pglnBufferHandle;
    }
    
    void Bind() const
    {
        glBindBuffer(
            GL_ARRAY_BUFFER, //%in need of abstracting.
            *m_pglnBufferHandle);
        if(GetOpenGLError(__FILE__, __LINE__))
            throw;
    }
    
private:
    shared_ptr<GLint> m_pglnBufferHandle;
};

//%In order to be scalable, we need multiple model instances reusing the buffer slot.
//%Also multiple distinct models stored in the same per buffer. 
//%This requires that a buffer slot store an offset and size.
//%Models themselves would need to be relocatable.
//%Model database stores: A map between model index and buffer slot (weak_ptr).
//%problem issuing top down draw. model doesn't know its physical object. 
//%current suggestion is: database<-(many)modelID<-(one)bufferslot<-(many)modelinstance;
//%(one)buffer<-(many)bufferslot

//%With more than one possible shader, there is a chance we will need the same model
//%stored in more than one way, to account for different vertex attributes.
//%There's also a chance that the same logical model would have more than one mesh,
//%say if different qualities are used.