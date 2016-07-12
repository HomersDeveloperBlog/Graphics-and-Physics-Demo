#pragma once

class Scene;

//%deprecated
std::tuple<GLuint, GLuint> SetupShaderIO();

//%deprecated
void DisplayTriangles(
	GLuint i_hArrayBuffer,
	GLuint i_hVertexArrayObject,
	GLuint i_nTriangleCount,
	const GLfloat * i_vertices);

void GameUpdateLoop(Scene & i_oScene);