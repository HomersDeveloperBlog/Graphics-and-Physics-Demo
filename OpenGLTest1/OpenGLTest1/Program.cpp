#include "Program.h"
#include "ShaderBuild.h"

#define OGL_INVALID_PROGRAM_HANDLE static_cast<GLint>(0)

OpenGLProgram::OpenGLProgram()
{
    GLuint glnProgramHandle = glCreateProgram();
    assert(glnProgramHandle > OGL_INVALID_PROGRAM_HANDLE);
	if(GetOpenGLError(__FILE__, __LINE__)
		|| glnProgramHandle <= OGL_INVALID_PROGRAM_HANDLE)
	{
		assert(!glIsProgram(glnProgramHandle));
		throw;
	}
    
    m_pglnProgramHandle = shared_ptr<GLint>(
        new GLint(glnProgramHandle),
        [](GLint * i_pglnProgramHandle)
        {
            glDeleteProgram(*i_pglnProgramHandle);
            assert(!GetOpenGLError(__FILE__, __LINE__));
                
            delete i_pglnProgramHandle;
        });
}

void OpenGLProgram::AttachShader(
    const OpenGLShader & i_oShader)
{
    glAttachShader(
        *m_pglnProgramHandle, 
        i_oShader.RawHandle());
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
}
    
void OpenGLProgram::DetachShader(
    const OpenGLShader & i_oShader)
{
    glDetachShader(
        *m_pglnProgramHandle, 
        i_oShader.RawHandle());
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
}
    
bool OpenGLProgram::GetLinkStatus()
{
    GLint glnLinkStatus = GL_FALSE;
    glGetProgramiv(
        *m_pglnProgramHandle, 
        GL_LINK_STATUS, 
        &glnLinkStatus);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
           
    return glnLinkStatus == GL_TRUE;
}
    
bool OpenGLProgram::Link()
{
    glLinkProgram(*m_pglnProgramHandle);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
    
    return GetLinkStatus();
}
    
string OpenGLProgram::GetLinkerMessage()
{
    GLint glnInfoLogLength = -1;
    glGetProgramiv(
        *m_pglnProgramHandle, 
        GL_INFO_LOG_LENGTH, 
        &glnInfoLogLength);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
                
    assert(glnInfoLogLength > 0);
    if(glnInfoLogLength > 0)
    {
        shared_ptr<char> pchInfoLogBuffer = shared_ptr<char>(
            new char[glnInfoLogLength],
            [](char * i_pBuffer)
            {
                delete [] i_pBuffer;
            });
                
        GLsizei nReturnedLength = 0;
        glGetProgramInfoLog(
            *m_pglnProgramHandle, 
            glnInfoLogLength, 
            &nReturnedLength, 
            pchInfoLogBuffer);
        assert(nReturnedLength == glnInfoLogLength);
        if(!GetOpenGLError(__FILE__, __LINE__))
            throw;
                    
        return string(pchInfoLogBuffer);
    }
    
    return string("<<empty message>>");
}

void OpenGLProgram::Use() const
{
    glUseProgram(*m_pglnProgramHandle);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
}

//%deprecated
//true if error; false otherwise.
bool LinkProgram(
	GLint i_glnProgramHandle)
{
	//Link program
	glLinkProgram(i_glnProgramHandle);
	if(GetOpenGLError(__FILE__, __LINE__))
    	return true;
	
	//Check link status
	GLint glnLinkStatus = GL_FALSE;
	glGetProgramiv(
		i_glnProgramHandle, 
		GL_LINK_STATUS, 
		&glnLinkStatus);
	if(GetOpenGLError(__FILE__, __LINE__))
		return true;

#ifdef _DEBUG
	//Dump linker message
	GLint glnInfoLogLength = -1;
	glGetProgramiv(
		i_glnProgramHandle, 
		GL_INFO_LOG_LENGTH, 
		&glnInfoLogLength);
	assert(glnInfoLogLength > 0);
	
	if(!GetOpenGLError(__FILE__, __LINE__)
		&& glnInfoLogLength > 0)
	{
		shared_ptr<char> pchInfoLogBuffer = 
			shared_ptr<char>(new (no_throw) char[glnInfoLogLength]);
		assert(pchInfoLogBuffer);
		
		if(pchInfoLogBuffer)
		{
			GLsizei nReturnedLength = 0;
			glGetProgramInfoLog(
				i_glnProgramHandle, 
				glnInfoLogLength, 
				&nReturnedLength, 
				pchInfoLogBuffer);
			assert(nReturnedLength == glnInfoLogLength);
			
			if(!GetOpenGLError(__FILE__, __LINE__))
			{
				cerr << pchInfoLogBuffer << endl;
			}
		}
	}
#endif //_DEBUG

	assert(glnLinkStatus == GL_TRUE);
	return glnLinkStatus == GL_FALSE;
}