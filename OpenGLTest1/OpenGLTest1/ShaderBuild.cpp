//ShaderBuild.cpp - Functions for compiling, linking, and using GLSL programs

#include "stdafx.h"
#include "assert.h"

#include <exception>

#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\freeglut.h"

#include "ShaderText.h"
#include "ShaderBuild.h"

//For glDebugMessageCallback, see:
//See https://blog.nobel-joergensen.com/2013/02/17/debugging-opengl-part-2-using-gldebugmessagecallback/

//%We want to (possibly manually), trace the name of the last gl api function called.
void TraceGLError()
{
	do //while(glError != GL_NO_ERROR)
	{
		GLint glnErrorCode = glError();
		assert(glnErrorCode == GL_NO_ERROR);
		
		switch(glnErrorCode)
		{
			case GL_NO_ERROR:
				cerr << "GL_NO_ERROR" << endl;
				break;
			case GL_INVALID_ENUM:
				cerr << "GL_INVALID_ENUM" << endl;
				break;
			case GL_INVALID_VALUE:
				cerr << "GL_INVALID_VALUE" << endl;
				break;
			case GL_INVALID_OPERATION:
				cerr << "GL_INVALID_OPERATION" << endl;
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl;
				break;
			case GL_OUT_OF_MEMORY:
				cerr << "GL_OUT_OF_MEMORY" << endl;
				break;
			case GL_STACK_UNDERFLOW:
				cerr << "GL_STACK_UNDERFLOW" << endl;
				break;
			case GL_STACK_OVERFLOW:
				cerr << "GL_STACK_OVERFLOW" << endl;
				break;
			case 0:
				cerr << "Error inside function: glError()." << endl;
				break;
		}
	} while(glError != GL_NO_ERROR);
}

int CompileShaderFromSource(
	const OpenGLShaderSource & i_oShaderSource)
{
	//Create shader object
	GLuint glnVertexShaderObject = glCreateShader(i_oShaderSource.m_glnShaderType);
	assert(glnVertexShaderObject > 0);
	TraceGLError(); //%need to abort if this happens as well.
	//%on abort, we need to delete the shader.
	
	//Load shader text
	glShaderSource(
		glnVertexShaderObject,
		1, //one source buffer only
		i_oShaderSource.m_strShaderSource.data(),
		0); //no length given; string is null terminated
	TraceGLError();
	
	//Compile shader
	glCompileShader(glnVertexShaderObject);
	TraceGLError();
	
	//Check compile status
	GLint glnCompileStatus = GL_FALSE;
	glGetShaderiv(
		glnVertexShaderObject, 
		GL_COMPILE_STATUS, 
		&glnCompileStatus);
	assert(glnCompileStatus == GL_TRUE);
	TraceGLError();
	
	//Dump compiler message
	GLint glnInfoLogLength = -1;
	glGetShaderiv(
		glnVertexShaderObject, 
		GL_INFO_LOG_LENGTH, 
		&glnInfoLogLength);
	assert(glnInfoLogLength > 0);
	TraceGLError();
		
	shared_ptr<char> pchInfoLogBuffer = 
		shared_ptr<char>(new (no_throw) char[glnInfoLogLength]);
	assert(pchInfoLogBuffer);
		
	if(pchInfoLogBuffer
		&& GLint > 0)
	{
		GLsizei nReturnedLength = 0;
		glGetShaderInfoLog(
			glnVertexShaderObject, 
			glnInfoLogLength, 
			&nReturnedLength, 
			pchInfoLogBuffer);
		assert(nReturnedLength == glnInfoLogLength);
		TraceGLError();
			
		cerr << pchInfoLogBuffer << endl;
	}
		
	if(glnCompileStatus == GL_FALSE)
	{
		cerr << "Error compiling shader." << endl;
		throw exception();
	}
}

void BuildOpenGLProgram(
	const OpenGLProgramManifest & i_oManifest)
{
	//Load shader sources
	OpenGLProgramSource oProgramSource;
	for(const OpenGLShaderFile & oShaderFile 
		: i_oManifest.m_vectShaders)
	{
		OpenGLProgramSource oShaderSource;
		
		if(oShaderFile.m_eShaderTypeInternal > 0U
			&& oShaderFile.m_eShaderTypeInternal < INTERNAL_SUPPORTED_SHADER_COUNT)
		{
			oShaderSource.m_glnShaderType = ShaderEnumInternalToOGL[
				oShaderFile.m_eShaderTypeInternal];
		}
		else
		{
			throw invalid_argument("Bad shader source enum.");
		}
		
		oShaderSource.m_strShaderSource = ReadFileToBuffer(
			oShaderFile.m_strShaderFileName);
	}
	
	//Compile shaders
	for(const OpenGLShaderSource oShaderSource 
		: oProgramSource.m_vectShaderSources)
	{
		
	}
	
	//%we have to be able to clean up handles if we fail to compile.  
	
	//Create program
	
	//Attach shaders
	
	//Link program
	
	//Detatch and delete all shaders. If there was an exception, now delete the program.
	
}

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
	//if(glnCompileStatus == GL_FALSE)
	//{
	//	GLint glnInfoLogLength = INVALID_LENGTH;
	//	glGetShaderiv(glnVertexShaderObject, GL_INFO_LOG_LENGTH, &glnInfoLogLength);
	//	assert(glnInfoLogLength > 0);

	//	char * chInfoLogBuffer = new char[glnInfoLogLength];
	//	int nReturnedLength = INVALID_LENGTH;
	//	glGetShaderInfoLog(glnVertexShaderObject, glnInfoLogLength, &nReturnedLength, chInfoLogBuffer);
	//	printf("%s", chInfoLogBuffer);

	//	exit(1);
	//}

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