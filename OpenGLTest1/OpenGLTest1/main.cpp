#include "stdafx.h"

#include "assert.h"
#include <tuple>

#include "GLBase.h"
#include "Meshes.h"
#include "Physics.h"
#include "GLSLBuild.h"
#include "VertexArrayObject.h"
#include "DrawScene.h"

using namespace std;

void InitializeOpenGLEnvironment()
{
	//Initialize OpenGL Utility (set basic settings, create a window)
	int nArgc = 0; char ** astrArgv = 0;
	glutInit(&nArgc, astrArgv);
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
	OpenGLVertexArrayObject oVAO;
	oVAO.Bind();

	GLuint glnLayoutNumber = oProgram.GetAttributeLocation(
		"vPosition");
	oVAO.DefineAttributeSourceFormat(
		glnLayoutNumber,
		2, 0); //%this information should come from somewhere

	//Load assets
	Model oModel(
		OpenGLBuffer(
			anMeshByteCount[SQUARE_MESH_2D],
			apMeshes[SQUARE_MESH_2D]),
		identity_matrix<double>(3U));

	//Create objects
	PhysicalObject oPendulum(
		oModel,
		PhysicalIntrinsicState(
			4.0, //mass
			identity_matrix<double>(3U)), //inertia
		PhysicalExtrinsicState(
			zero_vector<double>(3U), //position
			0.01f * unit_vector<double>(3U, 0U), //velocity
			identity_matrix<double>(3U), //angular position
			zero_vector<double>(3U))); //angular velocity
	
	//Create scene, enter update loop
	Scene oScene;
	oScene.AddObject(oPendulum);
	GameUpdateLoop(oScene); //%shove into scene.
	
	//glutDisplayFunc(UpdateDisplay);
	//glutMainLoop();

	return 0;
}
