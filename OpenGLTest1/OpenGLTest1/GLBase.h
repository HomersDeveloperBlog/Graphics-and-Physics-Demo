#pragma once

#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\freeglut.h"

#include <string>

#define OGL_INVALID_PROGRAM_HANDLE static_cast<GLint>(0)
#define OGL_MAX_EXPECTED_SHADER_COUNT static_cast<GLsizei>(5)
#define OGL_INVALID_SHADER_HANDLE static_cast<GLint>(0)

bool GetOpenGLError(
	const std::string & i_strFileName,
	int nLineNumber);