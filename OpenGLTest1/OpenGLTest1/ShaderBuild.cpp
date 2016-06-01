//ShaderBuild.cpp - Functions for compiling, linking, and using GLSL programs

#include "stdafx.h"
#include "assert.h"

#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\freeglut.h"

#include "ShaderText.h"
#include "ShaderBuild.h"

//%This needs to be refactored into a loop over shaders, with error handling / log dumping :)
//%need to import shaders from file, or at least put them in different files.
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