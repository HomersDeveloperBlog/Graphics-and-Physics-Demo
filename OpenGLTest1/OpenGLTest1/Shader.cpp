#include "stdafx.h"
#include "assert.h"

#include "GLSLBuild.h"
#include "Shader.h"

#define OGL_INVALID_SHADER_HANDLE static_cast<GLint>(0)

OpenGLShader::OpenGLShader(
    const OpenGLShaderSource & i_oShaderSource) 
{
    //Create shader object
    GLuint glnShaderHandle = glCreateShader(
        i_oShaderSource.m_glnShaderType);
        
    assert(glnShaderHandle > OGL_INVALID_SHADER_HANDLE);
    if(GetOpenGLError(__FILE__, __LINE__)
        || glnShaderHandle <= OGL_INVALID_SHADER_HANDLE)
    {
        assert(!glIsShader(glnShaderHandle));
        throw;
    }
	    
    //Create shared pointer to handle, set destructor
    m_pglnShaderHandle = shared_ptr<GLint>(
        new GLint(glnShaderHandle),
        [](GLint * i_pglnShaderHandle)
        {
            glDeleteShader(*i_pglnShaderHandle);
            assert(!GetOpenGLError(__FILE__, __LINE__));
                
            delete i_pglnShaderHandle;
            i_pglnShaderHandle = 0;
        });
        
    //Load shader text
    glShaderSource(
        *m_pglnShaderHandle,
        1, //one source buffer only
        i_oShaderSource.m_strShaderSource.data(),
        0); //no length given; string is null terminated
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
}
    
//True on success, false otherwise.
bool OpenGLShader::GetCompileStatus() 
{
    GLint glnCompileStatus = GL_FALSE;
    glGetShaderiv(
        *m_pglnShaderHandle, 
        GL_COMPILE_STATUS, 
        &glnCompileStatus);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
            
    return (glnCompileStatus == GL_TRUE);
}
    
//True on success, false otherwise.
bool OpenGLShader::Compile()
{
    glCompileShader(*m_pglnShaderHandle);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
    
    return GetCompileStatus();
}
     
string OpenGLShader::GetCompilerMessage()
{
    GLint glnInfoLogLength = -1;
    glGetShaderiv(
        *m_pglnShaderHandle, 
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
        glGetShaderInfoLog(
            *m_pglnShaderHandle, 
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
    
//%deprecated.
//API wrapper implemented in C-style. God help us all.
//Non-zero valid shader handle on success, 0 otherwise.
GLint CompileShaderFromSource(
	const OpenGLShaderSource & i_oShaderSource)
{
	//Create shader object
	GLuint glnShaderHandle = glCreateShader(i_oShaderSource.m_glnShaderType);
	assert(glnShaderHandle > OGL_INVALID_SHADER_HANDLE);
	if(GetOpenGLError(__FILE__, __LINE__)
		|| glnShaderHandle <= OGL_INVALID_SHADER_HANDLE)
	{
		assert(!glIsShader(glnShaderHandle));
		return OGL_INVALID_SHADER_HANDLE;
	}
	
	//Load shader text
	glShaderSource(
		glnShaderHandle,
		1, //one source buffer only
		i_oShaderSource.m_strShaderSource.data(),
		0); //no length given; string is null terminated
	if(GetOpenGLError(__FILE__, __LINE__))
	{
		glDeleteShader(glnShaderHandle);
		assert(!GetOpenGLError(__FILE__, __LINE__));
		return OGL_INVALID_SHADER_HANDLE;
	}
	
	//Compile shader
	glCompileShader(glnShaderHandle);
	if(GetOpenGLError(__FILE__, __LINE__))
	{
		glDeleteShader(glnShaderHandle);
		assert(!GetOpenGLError(__FILE__, __LINE__));
		return OGL_INVALID_SHADER_HANDLE;
	}
		
	//Check compile status
	GLint glnCompileStatus = GL_FALSE;
	glGetShaderiv(
		glnShaderHandle, 
		GL_COMPILE_STATUS, 
		&glnCompileStatus);
	if(GetOpenGLError(__FILE__, __LINE__))
	{
		glDeleteShader(glnShaderHandle);
		assert(!GetOpenGLError(__FILE__, __LINE__));
		return OGL_INVALID_SHADER_HANDLE;
	}

#ifdef _DEBUG
	//Dump compiler message
	GLint glnInfoLogLength = -1;
	glGetShaderiv(
		glnShaderHandle, 
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
			glGetShaderInfoLog(
				glnShaderHandle, 
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

	assert(glnCompileStatus == GL_TRUE);
	if(glnCompileStatus == GL_FALSE)
	{
		glDeleteShader(glnShaderHandle);
		assert(!GetOpenGLError(__FILE__, __LINE__));
		return OGL_INVALID_SHADER_HANDLE;
	}
	
	return glnShaderHandle;
}