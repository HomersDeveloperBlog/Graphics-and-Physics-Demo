#include "stdafx.h"
#include "assert.h"
#include <tuple>

#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\freeglut.h"

#include "Physics.h"
#include "GLSLBuild.h"
#include "DrawScene.h"

int main(
	int argc, 
	char ** argv)
{
	//Initialize OpenGL environment, using glut, glew
	InitializeOpenGLEnvironment();
	
	//Build program from shader files.
	OpenGLProgramManifest oManifest;
	oManifest.m_vectShaderFiles.push_back(OpenGLShaderFile(
		VERTEX_SHADER_INTERNAL,
		"triangles.vert"));
	oManifest.m_vectShaderFiles.push_back(OpenGLShaderFile(
		FRAGMENT_SHADER_INTERNAL,
		"triangles.frag"));
	
	OpenGLProgramSource oProgramSource(oManifest);
	OpenGLProgram oProgram = oProgramSource.BuildProgram();
	
	//Set vertex attributes.
	OpenGLVertexArrayObject m_oVAO;
	m_oVAO.Bind();
	
	//Load assets.
	
	//CompileShaders();
	
	//Create scene, populate objects.
	
	GameUpdateLoop(); //%input a scene
	
	//glutDisplayFunc(UpdateDisplay);
	glutMainLoop();

	return 0;
}
