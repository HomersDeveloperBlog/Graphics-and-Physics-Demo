#pragma once

#include <tuple>

using namespace std;

void InitializeOpenGLEnvironment();

tuple<GLuint, GLuint> SetupShaderIO();

void DisplayTriangles(
	GLuint hArrayBuffer,
	GLuint hVertexArrayObject,
	GLint i_nTriangleCount,
	const GLfloat * i_vertices);

void DrawLoop();