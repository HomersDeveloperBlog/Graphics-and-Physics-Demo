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

#define OGL_INVALID_SHADER_HANDLE static_cast<GLint>(0)
#define OGL_INVALID_PROGRAM_HANDLE static_cast<GLint>(0)

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

//API wrapper implemented in C-style. God help us all.
//Non-zero valid shader handle on success, 0 otherwise.
GLint CompileShaderFromSource(
	const OpenGLShaderSource & i_oShaderSource)
{
	//%Should be split into CreateShader and CompileShader.
	
	//%An obvious improvement would be to use RAII for destroying the shader handle.
	//%Proabably exceptions would make more sense here than error codes.
	
	//%Also theres an inconsistent mix of tracing, assertion, error codes, and (not) exceptions here.
	//%I'm thinking we can do any combination of all three.
	//%If(OGLException e = GetOpenGLErrorState())
	//{
	//	RaiseException(e, __FILE__, __LINE__);
	//}
	//%Based on compiler flags, RaiseException will trace, assert, throw;
	//%OpenGL should be able to continue any error.
	//%This code doesn't skip unless we define some conversion to bool. 
	//%Note the code doesn't have to be this simple.
	//%We may want to support a context message.
	
	//Create shader object
	GLuint glnVertexShaderObject = glCreateShader(i_oShaderSource.m_glnShaderType);
	assert(glnVertexShaderObject > OGL_INVALID_SHADER_HANDLE);
	if(GetOpenGLError(__FILE__, __LINE__)
		|| glnVertexShaderObject <= OGL_INVALID_SHADER_HANDLE)
	{
		assert(!glIsShader(glnVertexShaderObject));
		return OGL_INVALID_SHADER_HANDLE;
	}
	
	//Load shader text
	glShaderSource(
		glnVertexShaderObject,
		1, //one source buffer only
		i_oShaderSource.m_strShaderSource.data(),
		0); //no length given; string is null terminated
	if(GetOpenGLError(__FILE__, __LINE__))
	{
		glDeleteShader(glnVertexShaderObject);
		assert(!GetOpenGLError(__FILE__, __LINE__));
		return OGL_INVALID_SHADER_HANDLE;
	}
	
	//Compile shader
	glCompileShader(glnVertexShaderObject);
	if(GetOpenGLError(__FILE__, __LINE__))
	{
		glDeleteShader(glnVertexShaderObject);
		assert(!GetOpenGLError(__FILE__, __LINE__));
		return OGL_INVALID_SHADER_HANDLE;
	}
		
	//Check compile status
	GLint glnCompileStatus = GL_FALSE;
	glGetShaderiv(
		glnVertexShaderObject, 
		GL_COMPILE_STATUS, 
		&glnCompileStatus);
	if(GetOpenGLError(__FILE__, __LINE__))
	{
		glDeleteShader(glnVertexShaderObject);
		assert(!GetOpenGLError(__FILE__, __LINE__));
		return OGL_INVALID_SHADER_HANDLE;
	}

#ifdef _DEBUG
	//Dump compiler message
	GLint glnInfoLogLength = -1;
	glGetShaderiv(
		glnVertexShaderObject, 
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
				glnVertexShaderObject, 
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
		glDeleteShader(glnVertexShaderObject);
		assert(!GetOpenGLError(__FILE__, __LINE__));
		return OGL_INVALID_SHADER_HANDLE;
	}
	
	return glnVertexShaderObject;
}

//%mostly a copy of compile shader, except it doesn't own the program,
//%so it doesn't delete it on error.
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

GLint ConvertInternalShaderEnumToOGL(
    ShaderType i_eShaderType)
{
    assert(i_eShaderType > 0U
		&& i_eShaderType < INTERNAL_SUPPORTED_SHADER_COUNT);
	if(i_eShaderType > 0U
		&& i_eShaderType < INTERNAL_SUPPORTED_SHADER_COUNT)
	{
		return ShaderEnumInternalToOGL[i_eShaderType];
	}
    //%else throw
	
    return -1;
}

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
	
	//%wrap what follows into a function returning a flag.
	//%The highly redundant program deletion code should be tied to program destructor.
	
	//Create, compile, and attach shaders
	vector<GLint> vectShaderHandles; //%currently, these are all leaked on exit.
	vectShaderHandles.reserve(oProgramSource.m_vectShaderSoruces.size());
	//%should probably be eliminated in favour of program.detachall();
	
	for(const OpenGLShaderSource oShaderSource 
		: oProgramSource.m_vectShaderSources)
	{
		//Create and compile shader.
		GLint glnShaderHandle = CompileShaderFromSource(oShaderSource);
		
		//%We have to be able to clean up handles if we fail to compile.
		//%I'm thinking if we stuff the handles into shared pointers, they will destruct properly.
		//%You have to use pointers, which have to be dynamic 
		//%Stack object manages shared pointer to gln handle,
		//%constructed with appropriate delete function (glDelete + pointer to handle delete).
		
		//%Should also probably encapsulate all this error checking.
		//%I'm eyeing both shader and program as good wrapping targets.
		//%Shader object should compile on construction. API offers compile status flag.
		//%Program object cannot link on construction. API offers link status flag.
		//%probably want a way to return compiler/linker message as string.
		
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
	if(LinkProgram(glnProgramHandle));
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
		if(GetOpenGLError(__FILE__, __LINE__))
		{
			glDeleteProgram(glnProgramHandle);
			assert(!GetOpenGLError(__FILE__, __LINE__));
			return OGL_INVALID_PROGRAM_HANDLE;
		}
		
		//%automatic deletion not implemented
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