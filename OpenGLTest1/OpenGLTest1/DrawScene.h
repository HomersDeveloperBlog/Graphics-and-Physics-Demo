#pragma once

#include <tuple>

void InitializeOpenGLEnvironment();

std::tuple<GLuint, GLuint> SetupShaderIO(); //%export necessary?

void DisplayTriangles( //%export necessary?
	GLuint hArrayBuffer,
	GLuint hVertexArrayObject,
	GLuint i_nTriangleCount,
	const GLfloat * i_aafTriangles);

void DrawLoop(); //%surely, this function has some inputs