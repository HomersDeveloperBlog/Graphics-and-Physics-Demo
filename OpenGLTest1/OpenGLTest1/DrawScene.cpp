//as;lfkja;lskdjf
//;lsdjlkd

#include "stdafx.h"
#include "assert.h"
#include <chrono>
#include <thread>

#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\freeglut.h"

#include "Physics.h" //%not sure about this architecture

#include "DrawScene.h"

using namespace std;

//Arrays for integer handles to opengl objects
enum Attrib_IDs { vPosition = 0 }; //An enum for the variable names/indices of the vertex shader.

void InitializeOpenGLEnvironment()
{
	//Initialize OpenGL Utility (set basic settings, create a window)
	int nArgc = 0; char ** astrArgv = 0;
	glutInit(&nArgc, astrArgv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512); //%can this go at the end before the window name?
	glutInitContextVersion(2, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Pendulum");
	//%Experiment with rearranging this and the glew init. See what dependencies there are.

	//Initialize OpenGL Extension Wrangler (link opengl functions)
	if (glewInit())
	{
		//%Replace this with your own error handling.
		//cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit(EXIT_FAILURE);
	}
}

std::tuple<GLuint, GLuint> SetupShaderIO()
{
	//Generate array buffers
	GLuint hArrayBuffer = 0;
	glGenBuffers(1, &hArrayBuffer); //now generate handles for buffers
	glBindBuffer(GL_ARRAY_BUFFER, hArrayBuffer); //set array buffer as active

	//Generate vertex array objects
	GLuint hVertexArrayObject = 0;
	glGenVertexArrays(1, &hVertexArrayObject); //generate handles for VA's
	glBindVertexArray(hVertexArrayObject); //set triangle VA as active

	//Set how "vposition" (layout location 0) in vertex shader loads from currently active buffer (vertices)
	const int nVectorDimension = 2;
	const int nVectorStride = 0;
	glVertexAttribPointer(
		vPosition,
		nVectorDimension,
		GL_FLOAT,
		GL_FALSE,
		nVectorStride,
		(void*)(0));
	glEnableVertexAttribArray(vPosition);

	return make_pair(hArrayBuffer, hVertexArrayObject);
}

void DisplayTriangles(
	GLuint hArrayBuffer,
	GLuint hVertexArrayObject,
	GLint i_nTriangleCount,
	const GLfloat * i_vertices)
{
	glClear(GL_COLOR_BUFFER_BIT);

	//set array buffer as active, copy 'vertices' into buffer
	glBindBuffer(GL_ARRAY_BUFFER, hArrayBuffer);
	glBindVertexArray(hVertexArrayObject);

	unsigned int nVertexCount = 3 * i_nTriangleCount;
	unsigned int nComponentCount = 2 * nVertexCount;
	glBufferData(GL_ARRAY_BUFFER, sizeof(*i_vertices) * nComponentCount, i_vertices, GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, nComponentCount);

	glFlush();
}

void DrawLoop()
{
	const unsigned int nTriangleCount = 2;
	GLfloat anTestVertices[6][2] = //6 vertices, 2 components each
	{
		{0.0f, 0.0f}, {0.1f, 0.0f}, {0.0f, 0.1f},
		{0.0f, 0.1f}, {0.1f, 0.0f}, {0.1f, 0.1f},
	};

	//Setup shader io
	GLuint hArrayBuffer, hVertexArrayObject;
	std::tie(hArrayBuffer, hVertexArrayObject) = SetupShaderIO();

	//Setup timers
	std::chrono::steady_clock::time_point nCurrentTime = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point nTargetTime = std::chrono::steady_clock::now();

	//Physical objects
	float fVelocity = 0.01f;
	PhysicalObject oPendulum(
		4.0,
		zero_matrix<double>(3U, 3U), 
		zero_vector<double>(3U), 
		fVelocity * unit_vector<double>(3U, 1U)); //v

	while(true)
	{
		//So one way of getting this to be timed is to make a thread that manages the time and then triggers events.
		//We have no need for a complex real-time event engine right now.
		while(nCurrentTime < nTargetTime)
		{
			nCurrentTime = std::chrono::steady_clock::now();
		}
		nTargetTime += std::chrono::milliseconds(10);
		
		//%need world space / model space / screen space conversion.
		//%eventually you need to use shaders for this, and only deal in world space in the code.
		//%need newtonian engine based on physical quantities with units and vector arithmetic.
		//%a simple physics would be to have repulsion from boundary; simple harmonic oscillator.

		oPendulum.AdvanceState(0.01);
		//%reproduce constant velocity evolution here.
		//%we need to construct the display scene from the model-space meshes.

		//Step forward triangle
		fVelocity += 0.0001f;

		anTestVertices[0][0] += fVelocity; //%normalize movement to time
		anTestVertices[1][0] += fVelocity;
		anTestVertices[2][0] += fVelocity;

		anTestVertices[3][0] += fVelocity; //%normalize movement to time
		anTestVertices[4][0] += fVelocity;
		anTestVertices[5][0] += fVelocity;

		//%need 'draw scene' that will draw scene based on a 'scene' object
		//%the physics code would update a 'world' object
		//%the scene object must be constructed from the world object.
		//%this is done by windowing the world
		//%an equivalence between world and scene can be used for now.

		DisplayTriangles(
			hArrayBuffer, 
			hVertexArrayObject, 
			nTriangleCount, 
			&anTestVertices[0][0]);
	}
}