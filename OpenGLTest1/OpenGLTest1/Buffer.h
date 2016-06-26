#pragma once

#include <memory>

#include "GLBase.h"

class OpenGLBuffer
{
public:
    OpenGLBuffer(size_t i_nSize, const void * i_pData);
    
    GLuint RawHandle() const {return *m_pglnBufferHandle;}
    
    void Bind() const;
    size_t Size() const;
    
private:
    std::shared_ptr<GLuint> m_pglnBufferHandle;
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