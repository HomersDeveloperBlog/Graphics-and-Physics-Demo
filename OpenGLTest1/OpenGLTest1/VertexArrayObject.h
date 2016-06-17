//%For now we only need one and we will bind it at the start of the program.

class OpenGLVertexArrayObject
{
    OpenGLVertexArrayObject();
    
    GLint RawHandle() const {return *m_pglnVAOHandle;}
    
    void Bind() const;
    void DefineAttributeSourceFormat(
        int i_nIndex,
        int i_nVectorDimension,
        int i_nStride);
    
private:
    shared_ptr<GLint> m_pglnVAOHandle;
};


