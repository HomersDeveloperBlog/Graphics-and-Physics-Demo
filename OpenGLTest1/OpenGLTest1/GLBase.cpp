//Diagnostics stuff for openGL

#include <iostream>
#include "assert.h"

#include "GLBase.h"

using namespace std;

//For glDebugMessageCallback, see:
//See https://blog.nobel-joergensen.com/2013/02/17/debugging-opengl-part-2-using-gldebugmessagecallback/

//%If(OGLException e = GetOpenGLErrorState())
//{
//	RaiseException(e, __FILE__, __LINE__);
//}
//%Based on compiler flags, RaiseException will trace, assert, throw;
//%OpenGL should be able to continue any error.
//%This code doesn't skip unless we define some conversion to bool. 
//%Note the code doesn't have to be this simple.
//%We may want to support a context message.

//Convert ogl error code to string.
string ConvertOGLErrorToString(
	GLint i_gnlErrorCode)
{
	string strReturnValue;
	switch(i_gnlErrorCode)
	{
		case GL_NO_ERROR:
			strReturnValue = "GL_NO_ERROR";
			break;
		case GL_INVALID_ENUM:
			strReturnValue = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			strReturnValue = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			strReturnValue = "GL_INVALID_OPERATION";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			strReturnValue = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			strReturnValue = "GL_OUT_OF_MEMORY";
			break;
		case GL_STACK_UNDERFLOW:
			strReturnValue = "GL_STACK_UNDERFLOW";
			break;
		case GL_STACK_OVERFLOW:
			strReturnValue = "GL_STACK_OVERFLOW";
			break;
		default:
			strReturnValue = "Unrecognized error code.";
	}
	
	return strReturnValue;
}

//false if error free.
bool GetOpenGLError(
	const string & i_strFileName,
	int nLineNumber)
{
	bool bReturnValue = false;
	GLint glnErrorCode = GL_NO_ERROR;

	do //while(glnErrorCode != GL_NO_ERROR)
	{
		glnErrorCode = glGetError();
		assert(glnErrorCode == GL_NO_ERROR);
		
		if(glnErrorCode != GL_NO_ERROR)
		{
			bReturnValue = true;
			
#ifdef _DEBUG
			cerr << "FILE: " << i_strFileName << endl;
			cerr << "LINE: " << nLineNumber << endl;
			cerr << "ERROR: " << ConvertOGLErrorToString(glnErrorCode) << endl;
#endif //_DEBUG

		}
	} while(glnErrorCode != GL_NO_ERROR);
	
	return bReturnValue;
}