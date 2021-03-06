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
	GLuint i_hArrayBuffer,
	GLuint i_hVertexArrayObject,
	GLint i_nTriangleCount,
	const GLfloat * i_vertices)
{
	glClear(GL_COLOR_BUFFER_BIT);

	//set array buffer as active, copy 'vertices' into buffer
	glBindBuffer(GL_ARRAY_BUFFER, i_hArrayBuffer);
	glBindVertexArray(i_hVertexArrayObject);

	unsigned int nVertexCount = 3U * i_nTriangleCount;
	unsigned int nComponentCount = 2U * nVertexCount;
	glBufferData(GL_ARRAY_BUFFER, sizeof(*i_vertices) * nComponentCount, i_vertices, GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, nComponentCount); //%last argument should be i_nTriangleCount?

	glFlush();
}

void DrawPhysicalObject(
	GLuint i_hArrayBuffer, //%wrap these two into a struct
	GLuint i_hVertexArrayObject,
	const PhysicalObject & i_oObject)
{
	//Call display triangles after extracting the raw triangle data pointer, and triangle count
	//Its not clear (to me) that the buffer object can be reused like this.
	//Vertex array object should be fine, so long as the format of the buffer hasn't changed.
	//Layout within a buffer could have alignment issues if using float on 64-bit system. Use nonzero stride to solve.
	
	DisplayTriangles(
		i_hArrayBuffer,
		i_hVertexArrayObject,
		i_oObject.m_oWorldSpaceMesh.m_vectTriangles.size(),
		i_oObject.m_oWorldSpaceMesh.m_vectTriangles.data()->data());
}

void DrawLoop()
{
	//const unsigned int nTriangleCount = 2U;
	//GLfloat anTestVertices[6][2] = //6 vertices, 2 components each
	//{
	//	{0.0f, 0.0f}, {0.1f, 0.0f}, {0.0f, 0.1f},
	//	{0.0f, 0.1f}, {0.1f, 0.0f}, {0.1f, 0.1f},
	//};

	//Setup shader io
	GLuint hArrayBuffer, hVertexArrayObject; //%wrap into struct
	std::tie(hArrayBuffer, hVertexArrayObject) = SetupShaderIO();

	//Setup timers
	std::chrono::steady_clock::time_point nCurrentTime = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point nTargetTime = std::chrono::steady_clock::now();

	//Physical objects
	float fVelocity = 0.01f;
	PhysicalObject oPendulum( //%constructor needs to be updated with display mesh
		4.0, //mass
		zero_matrix<double>(3U, 3U), 
		zero_vector<double>(3U), 
		fVelocity * unit_vector<double>(3U, 0U)); //v

	while(true)
	{
		//So one way of getting this to be timed is to make a thread that manages the time and then triggers events.
		//We have no need for a complex real-time event engine right now.
		while(nCurrentTime < nTargetTime)
		{
			nCurrentTime = std::chrono::steady_clock::now();
		}
		nTargetTime += std::chrono::milliseconds(10); //%this needs to tie in with advance state time
		
		//%need world space / model space / screen space conversion.
		//%eventually you need to use shaders for this, and only deal in world space in the code.
		//%need newtonian engine based on physical quantities with units and vector arithmetic.

		oPendulum.AdvanceState(0.01); //%this needs to tie with loop time

		////Step forward triangle
		//fVelocity += 0.0001f;

		//anTestVertices[0][0] += fVelocity; //%normalize movement to time
		//anTestVertices[1][0] += fVelocity;
		//anTestVertices[2][0] += fVelocity;

		//anTestVertices[3][0] += fVelocity; //%normalize movement to time
		//anTestVertices[4][0] += fVelocity;
		//anTestVertices[5][0] += fVelocity;

		//%need 'draw scene' that will draw scene based on a 'scene' object
		//%the physics code would update a 'world' object
		//%the scene object must be constructed from the world object.
		//%an equivalence between world and scene can be used for now.

		DrawPhysicalObject(
			hArrayBuffer,
			hVertexArrayObject,
			oPendulum);

		//DisplayTriangles(
		//	hArrayBuffer, 
		//	hVertexArrayObject, 
		//	nTriangleCount, 
		//	&anTestVertices[0][0]);
	}
}