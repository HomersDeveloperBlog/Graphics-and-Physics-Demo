#include "stdafx.h"
#include "assert.h"
#include <iostream>

#include "GLSLBuild.h"
#include "Shader.h" //%circular: AttachShader(const OpenGLShader &)
#include "Program.h"

using namespace std;

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
    
bool OpenGLProgram::GetLinkStatus() const
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
    
string OpenGLProgram::GetLinkerMessage() const
{
    GLint glnInfoLogLength = -1;
    glGetProgramiv(
        *m_pglnProgramHandle, 
        GL_INFO_LOG_LENGTH, 
        &glnInfoLogLength);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
                
    assert(glnInfoLogLength >= 1);
    if(glnInfoLogLength > 1)
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
            static_cast<GLchar *>(pchInfoLogBuffer.get()));
        assert(nReturnedLength == glnInfoLogLength);
        if(GetOpenGLError(__FILE__, __LINE__))
            throw;
                    
        return string(pchInfoLogBuffer.get());
    }
    
    return string("Linker: <<empty message>>");
}

void OpenGLProgram::Use() const
{
    glUseProgram(*m_pglnProgramHandle);
    if(GetOpenGLError(__FILE__, __LINE__))
        throw;
}

void OpenGLProgram::DetachAll()
{
    //Get attached shader count
    GLint glnAttachedShaderCount = -1;
    glGetProgramiv(
        *m_pglnProgramHandle, 
        GL_ATTACHED_SHADERS, 
        &glnAttachedShaderCount);
    assert(glnAttachedShaderCount >= 0);
    if(GetOpenGLError(__FILE__, __LINE__)
        || glnAttachedShaderCount < 0)
        throw;
    
    //Success if no shaders attached.
    if(!glnAttachedShaderCount)
       return;
       
    GLsizei glnShadersWritten = -1;
    GLuint aglnShaderHandles[OGL_MAX_EXPECTED_SHADER_COUNT] 
        = {OGL_INVALID_SHADER_HANDLE};
        
    glGetAttachedShaders(	
        *m_pglnProgramHandle,
        OGL_MAX_EXPECTED_SHADER_COUNT,
        &glnShadersWritten,
        aglnShaderHandles);
    assert(glnShadersWritten == glnAttachedShaderCount);
    if(GetOpenGLError(__FILE__, __LINE__)
        || glnShadersWritten != glnAttachedShaderCount)
        throw;
        
    for(int nShaderIndex = 0;
        nShaderIndex < glnShadersWritten;
        ++nShaderIndex)
    {
        assert(aglnShaderHandles[nShaderIndex] > 0);
        glDetachShader(
            *m_pglnProgramHandle, 
            aglnShaderHandles[nShaderIndex]);
        if(GetOpenGLError(__FILE__, __LINE__))
            throw;
    }
}

GLuint OpenGLProgram::GetAttributeLocation(
	const std::string & i_strName)
{
	GLint glnLayoutNumber = glGetAttribLocation(
		*m_pglnProgramHandle,
		static_cast<const GLchar *>(i_strName.data()));
	assert(glnLayoutNumber >= 0);
	if(GetOpenGLError(__FILE__, __LINE__)
		|| glnLayoutNumber < 0)
		throw;

	return static_cast<GLuint>(glnLayoutNumber);
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
			shared_ptr<char>(new (nothrow) char[glnInfoLogLength]);
		assert(pchInfoLogBuffer);
		
		if(pchInfoLogBuffer)
		{
			GLsizei nReturnedLength = 0;
			glGetProgramInfoLog(
				i_glnProgramHandle, 
				glnInfoLogLength, 
				&nReturnedLength, 
				static_cast<GLchar *>(pchInfoLogBuffer.get()));
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