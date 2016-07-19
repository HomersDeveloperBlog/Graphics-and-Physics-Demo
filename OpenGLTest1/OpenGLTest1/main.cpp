#include "stdafx.h"
#include "assert.h"
#include <tuple>
#include <string>

#include "GLBase.h"
#include "Model.h"
#include "Scene.h"
#include "Meshes.h"
#include "PhysicalObject.h"
#include "GLSLBuild.h"
#include "VertexArrayObject.h"
#include "DrawScene.h"

using namespace std;

void InitializeOpenGLEnvironment()
{
	//Initialize OpenGL Utility (set basic settings, create a window)
	int argc = 0;
	glutInit(&argc, nullptr);
	glutInitContextVersion(3, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGBA); 
	glutInitWindowSize(512, 512);
	glutCreateWindow("Physics and OpenGL Demo");

	//Initialize OpenGL Extension Wrangler (import opengl functions)
	if (glewInit())
		throw;
}

int main(
	int argc, 
	char ** argv)
{
	//	//Initialize OpenGL environment, using glut, glew
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
	oProgram.Use();

	//Load assets
	DisplayModel oDisplayModel(
		OpenGLBuffer(
			anMeshByteCount[SQUARE_MESH_2D],
			apMeshes[SQUARE_MESH_2D]),
			anMeshVertexCount[SQUARE_MESH_2D],
		identity_matrix<double>(3U));

	//Create objects
	PhysicalObject oPendulum(
		oDisplayModel,
		PhysicalIntrinsicState(
			4.0, //mass
			identity_matrix<double>(3U)), //inertia
		PhysicalExtrinsicState(
			zero_vector<double>(3U), //position
			zero_vector<double>(3U), //0.01f * unit_vector<double>(3U, 0U), //velocity
			identity_matrix<double>(3U), //angular position
			zero_vector<double>(3U))); //angular velocity
	
	//Create scene
	Scene oScene;
	oScene.AddObject(&oPendulum);

	//Set vertex attributes.
	OpenGLVertexArrayObject oVAO;
	oVAO.Bind();

	GLuint glnLayoutNumber = oProgram.GetAttributeLocation(
		"vPosition");
	oVAO.DefineAttributeSourceFormat(
		glnLayoutNumber,
		2, 0); //%this information should come from somewhere

	GameUpdateLoop(oScene);

	//glutDisplayFunc(UpdateDisplay);
	//glutMainLoop();

	return 0;
}