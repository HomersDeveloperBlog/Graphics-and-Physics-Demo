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

//false if error free.
bool GetOpenGLError(
	const string & i_strFileName,
	int nLineNumber)
{
	bool bReturnValue = false;
	do //while(glError != GL_NO_ERROR)
	{
		GLint glnErrorCode = glError();
		assert(glnErrorCode == GL_NO_ERROR);
		
		if(glnErrorCode != GL_NO_ERROR)
		{
			bReturnValue = true;
#ifdef _DEBUG
			cerr << "FILE: " << i_strFileName << endl;
			cerr << "LINE: " << nLineNumber << endl;
			
			//Convert ogl error code to string.
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
#endif //_DEBUG
		}
	} while(glError != GL_NO_ERROR);
	
	return bReturnValue;
}

//Succeeds and builds a program you can use, or throws an exception.
OpenGLProgram BuildProgramFromManifest(
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
	//%probably want a way to return compiler/linker message as string.
	//%should not stop us from detaching/deleting shaders.
	//%defer error handling until after cleanup.
	//%in RAII'd version, use return value to decide deletion.
	if(oProgram.Link()))
	{
		glDeleteProgram(glnProgramHandle);
		assert(!GetOpenGLError(__FILE__, __LINE__));
		return OGL_INVALID_PROGRAM_HANDLE; 
	}
	
	//Detatch all shaders. Deletion will occur at scope exit.
	for(const GLint glnShaderHandle
		: vectShaderHandles)
	{
		oProgram.Detach(glnShaderHandle);
	}
	
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
	//%probably want a way to return compiler/linker message as string.
	//%should not stop us from detaching/deleting shaders.
	//%defer error handling until after cleanup.
	//%in RAII'd version, use return value to decide deletion.
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