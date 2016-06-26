#pragma once

#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\freeglut.h"

#include <string>

bool GetOpenGLError(
	const std::string & i_strFileName,
	int nLineNumber);