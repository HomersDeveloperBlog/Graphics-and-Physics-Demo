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
	glutInitDisplayMode(GLUT_RGBA); //%and this
	glutInitWindowSize(512, 512); //%can this go at the end before the window name?
	glutInitContextVersion(2, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
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
	
	//Set vertex attributes.
	OpenGLVertexArrayObject oVAO;
	oVAO.Bind();

	GLuint glnLayoutNumber = oProgram.GetAttributeLocation(
		"vPosition");
	oVAO.DefineAttributeSourceFormat(
		glnLayoutNumber,
		2, 0); //%this information should come from somewhere

	//Load assets
	DisplayModel oDisplayModel(
		OpenGLBuffer(
			anMeshByteCount[SQUARE_MESH_2D],
			apMeshes[SQUARE_MESH_2D]),
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
	
	//Create scene, enter update loop
	Scene oScene;
	oScene.AddObject(&oPendulum);
	
	GLuint hBuffer, hVAO;
	tie(hBuffer, hVAO) = SetupShaderIO();

	//%looks like one of these doesn't work. im guessing buffer.
	//DisplayTriangles(
	//	hBuffer, //oDisplayModel.Buffer().RawHandle(),
	//	hVAO, 
	//	2,
	//	apMeshes[SQUARE_MESH_2D]);
	
	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glBindVertexArray(hVAO);

	unsigned int nVertexCount = 3U * 2U;
	unsigned int nComponentCount = 2U * nVertexCount;
	unsigned int nBufferSize = sizeof(*apMeshes[SQUARE_MESH_2D]) * nComponentCount;
	glBufferData(
		GL_ARRAY_BUFFER,
		nBufferSize,
		apMeshes[SQUARE_MESH_2D],
		GL_STATIC_DRAW);

	//oVAO.Bind();
	//oDisplayModel.Buffer().Bind();

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(
		GL_TRIANGLES,
		0, //Start index
		nComponentCount); //%previously float count
	glFlush();

	GameUpdateLoop(oScene);
	
	//%Something with the buffer code is all fucked up.
	//%DrawTriangles (deprecated) has been inlined here.
	//%It works fine but for some reason the refactored version does not.
	//%Using both the new VAO and buffer code results in an access violation.
	//%Try to analyze the buffer to reading data out of it?
	//New buffer seems to work just fine when bound after both buffers are defined.
	//And not if it is bound before the deprecated buffer is defined
	//When the new VAO is bound, nothing is drawn and no exception occurs.
	//This is probably because no data is being read.

	//%also fragment shader colour doesn't seem to be responding properly.

	//glutDisplayFunc(UpdateDisplay);
	//glutMainLoop();

	return 0;
}