//ShaderBuild.cpp - Functions for compiling, linking, and using GLSL programs

#include "stdafx.h"
#include "assert.h"

#include <exception>

#include "ShaderText.h" //%deprecated
#include "GLSLBuild.h"

const static GLint aglnShaderEnumInternalToOGL[
	INTERNAL_SUPPORTED_SHADER_COUNT] = 
{
    GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
};

GLint OpenGLShaderSource::ConvertInternalShaderEnumToOGL(
	ShaderType i_eShaderType)
{
	assert(i_eShaderType >= 0
		&& i_eShaderType < INTERNAL_SUPPORTED_SHADER_COUNT);
	if(i_eShaderType < 0
		&& i_eShaderType >= INTERNAL_SUPPORTED_SHADER_COUNT)
		throw;
	
	return aglnShaderEnumInternalToOGL[i_eShaderType];
}

OpenGLShaderSource::OpenGLShaderSource(
    const OpenGLShaderFile & oShaderFile)
{
    m_glnShaderType = ConvertInternalShaderEnumToOGL(
		oShaderFile.m_eShaderTypeInternal);
	m_strShaderSource = ReadFileToBuffer(
		oShaderFile.m_strShaderFileName);
}
	
OpenGLProgramSource::OpenGLProgramSource(
	const OpenGLProgramManifest & i_oManifest)
{
    for(const OpenGLShaderFile & oShaderFile 
        : i_oManifest.m_vectShaders)
    {
        OpenGLShaderSource oShaderSource(oShaderFile);
        m_vectShaderSources.push_back(oShaderSource);
    }
}

//Succeeds and builds a program you can use, or throws an exception.
OpenGLProgram OpenGLProgramSource::BuildProgram()
{
	//Create program
	OpenGLProgram oProgram;
	
	//Create, compile, and attach shaders
	for(const OpenGLShaderSource oShaderSource 
		: m_vectShaderSources)
	{
		//Create shader
		OpenGLShader oShader(oShaderSource); 

		//Compile shader
		if(!oShader.Compile())
			throw;
			
#ifdef _DEBUG
		//Print compiler message
		cerr << oShader.GetCompilerMessage() << endl;
#endif //_DEBUG
		
		//Attach shader
		oProgram.Attach(oShader);
	}
	
	//Link program
	if(!oProgram.Link()))
		throw;
		
#ifdef _DEBUG
	//Print linker message
	cerr << oProgram.GetLinkerMessage() << endl;
#endif //_DEBUG

	//Detatch all shaders.
	oProgram.DetachAll();
	
	return oProgram;
}

//%deprecated.
GLint BuildOpenGLProgram(
	const OpenGLProgramManifest & i_oManifest)
{
	//Load shader sources
	OpenGLProgramSource oProgramSource; //%make this a constructor.
	for(const OpenGLShaderFile & oShaderFile 
		: i_oManifest.m_vectShaders)
	{
		OpenGLProgramSource oShaderSource; //%make this a constructor?
		oShaderSource.m_glnShaderType = ConvertInternalShaderEnumToOGL(
			oShaderFile.m_eShaderTypeInternal);
		oShaderSource.m_strShaderSource = ReadFileToBuffer(
			oShaderFile.m_strShaderFileName);
		//%read return values.
		
		oProgramSource.m_vectShaderSources.push_back(oShaderSource);
	}
	
	//Create program
	GLuint glnProgramHandle = glCreateProgram();
	assert(glnProgramHandle > OGL_INVALID_PROGRAM_HANDLE);
	if(GetOpenGLError(__FILE__, __LINE__)
		|| glnProgramHandle <= OGL_INVALID_PROGRAM_HANDLE)
	{
		assert(!glIsProgram(glnProgramHandle));
		return OGL_INVALID_PROGRAM_HANDLE;
	}
	
	//Create, compile, and attach shaders
	vector<GLint> vectShaderHandles; //%currently, these are all leaked on exit.
	vectShaderHandles.reserve(oProgramSource.m_vectShaderSoruces.size());
	//%should probably be eliminated in favour of program.detachall();
	
	for(const OpenGLShaderSource oShaderSource 
		: oProgramSource.m_vectShaderSources)
	{
		//Create and compile shader.
		GLint glnShaderHandle = CompileShaderFromSource(oShaderSource);
		if(glnShaderHandle == OGL_INVALID_SHADER_HANDLE)
		{
			glDeleteProgram(glnProgramHandle);
			assert(!GetOpenGLError(__FILE__, __LINE__));
			return OGL_INVALID_PROGRAM_HANDLE;
		}

		//Attach shader
		glAttachShader(glnProgramHandle, glnShaderHandle);
		if(GetOpenGLError(__FILE__, __LINE__))
		{
			glDeleteProgram(glnProgramHandle);
			assert(!GetOpenGLError(__FILE__, __LINE__));
			return OGL_INVALID_PROGRAM_HANDLE;
		}
	
		vectShaderHandles.push_back(glnShaderHandle);
	}
	
	//Link program
	if(LinkProgram(glnProgramHandle))
	{
		glDeleteProgram(glnProgramHandle);
		assert(!GetOpenGLError(__FILE__, __LINE__));
		return OGL_INVALID_PROGRAM_HANDLE; 
	}
	
	//Detatch all shaders. Deletion will occur at scope exit.
	for(const GLint glnShaderHandle
		: vectShaderHandles)
	{
		glDetachShader(glnProgramHandle, glnShaderHandle);
		assert(!GetOpenGLError(__FILE__, __LINE__));
		
		//%automatic deletion not implemented
		glDeleteShader(glnShaderHandle);
		assert(!GetOpenGLError(__FILE__, __LINE__));
	}
	
	return glnProgramHandle;
}

//%deprecated.
void CompileShaders()
{
	//Compile shaders, starting with vertex shader object
	GLuint glnVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	assert(glnVertexShaderObject > 0);

	glShaderSource(glnVertexShaderObject, 1, &astrShaderSources[eVertexShaderSource], 0);
	glCompileShader(glnVertexShaderObject);

	GLint glnCompileStatus = -555;
	glGetShaderiv(glnVertexShaderObject, GL_COMPILE_STATUS, &glnCompileStatus);
	assert(glnCompileStatus == GL_TRUE);

	//Compile fragment shader object
	GLuint glnFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	assert(glnFragmentShaderObject > 0);

	glShaderSource(glnFragmentShaderObject, 1, &astrShaderSources[eFragmentShaderSource], 0);
	glCompileShader(glnFragmentShaderObject);

	glnCompileStatus = -555;
	glGetShaderiv(glnFragmentShaderObject, GL_COMPILE_STATUS, &glnCompileStatus);
	assert(glnCompileStatus == GL_TRUE);

	//Link the program
	GLuint glnProgramObject = glCreateProgram();
	assert(glnProgramObject > 0);

	glAttachShader(glnProgramObject, glnVertexShaderObject);
	glAttachShader(glnProgramObject, glnFragmentShaderObject);
	glLinkProgram(glnProgramObject);

	glnCompileStatus = -555;
	glGetProgramiv(glnProgramObject, GL_LINK_STATUS, &glnCompileStatus);
	assert(glnCompileStatus == GL_TRUE);

	//Use the program
	glUseProgram(glnProgramObject);
}