//An instance of an drawable object.

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

class OpenGLVertexArrayObject
{
    OpenGLVertexArrayObject()
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
    
    GLint RawHandle() const 
    {
        return *m_pglnVAOHandle;
    }
    
    void Bind() const
    {
        glBindVertexArray(*m_pglnVAOHandle);
        if(GetOpenGLError(__FILE__, __LINE__))
            throw;
    }
    
    //Lookup is GLint glGetAttribLocation(GLuint program, const GLchar *name);
    void DefineAttributeSourceFormat(
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
            i_nIndex, //layout number
            i_nVectorDimension, //as it is packed in the buffer.
            GL_FLOAT, //buffer type (converted from this type to foat)
            GL_FALSE, //normalize?
            i_nStride,
            (void*)(0)); //offset from start of bound GL_ARRAY_BUFFER
        if(GetOpenGLError(__FILE__, __LINE__))
            throw;
        
        //Enable attribute
        glEnableVertexArrayAttrib(
            *m_pglnVAOHandle,
 	        i_nIndex);
        if(GetOpenGLError(__FILE__, __LINE__))
            throw;
    }
    
private:
    shared_ptr<GLint> m_pglnVAOHandle;
};

//%A vertex attribute is conversion layer between the buffer and shader input.
//%For example, a buffer of a 2D vertices to a 3D position-only shader.
//%In matching the two, we can use a list of all possible attributes. 
//%Misses are handled fine. Buffer data that the shader cannot accept is unused.
//%Shader input the buffer does not provide is zeroed by opengl.
//%Each buffer attribute has a start pos, size and stride. Shader input has layout index.
//%For obscure shaders/buffers, its going to be hard to avoid O(nm) VAO definitions.
//%I'm thinking a generic VAO class that handles memory management,
//%and then have a virtual SetAttributePointers. 

//%Each model storing its VAO together with its buffer should be fine. 
//%In a more advanced top-down drawing, the buffer would be looked up, 
//%and the VAO would depend on the shader being used.

//%Ultimately, we would have to have a database of VAOs that are queried by 
//%a buffer format and shader format.
//%For now we only need one and we will bind it at the start of the program.

class Model
{
public:
	Model(c_matrix<double, 3U, 3U> i_aadModelToWorld)
		: m_aadModelToWorld(i_aadModelToWorld) {}
	
	//The model to world matrix for the initial scaling about arbitrary frame
	const c_matrix<double, 3U, 3U> & ScalingMatrix() const
	{
		return m_aadScaling;
	}
	
private:
	OpenGLBuffer m_hBuffer;
	c_matrix<double, 3U, 3U> m_aadScaling;
};