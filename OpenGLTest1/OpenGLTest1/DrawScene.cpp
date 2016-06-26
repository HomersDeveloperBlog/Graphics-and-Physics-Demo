#include "stdafx.h"

#include "assert.h"
#include <chrono>
#include <thread>
#include <tuple> //%deprecated

#include "GLBase.h"
#include "Scene.h"
#include "PhysicalObject.h"
#include "DrawScene.h"

using namespace std;

//%deprecated
tuple<GLuint, GLuint> SetupShaderIO()
{
	//Generate array buffers
	GLuint hArrayBuffer = 0;
	glGenBuffers(1, &hArrayBuffer); //now generate handles for buffers
	glBindBuffer(GL_ARRAY_BUFFER, hArrayBuffer); //set array buffer as active

	//Generate vertex array objects
	GLuint hVertexArrayObject = 0;
	glGenVertexArrays(1, &hVertexArrayObject); //generate handles for VA's
	glBindVertexArray(hVertexArrayObject); //set triangle VA as active

	//glGetAttribLocation();
	//Set how "vposition" (layout location 0) in vertex shader loads from currently active buffer (vertices)
	const int nVectorDimension = 2;
	const int nVectorStride = 0;
	glVertexAttribPointer( //Targets float data input to vertex shader. use 'L' for double
		0, //layout number
		nVectorDimension, //as it is packed in the buffer.
		GL_FLOAT, //buffer type (converted from this type to foat)
		GL_FALSE, //normalize?
		nVectorStride,
		(void*)(0)); //offset from start of bound GL_ARRAY_BUFFER
	glEnableVertexAttribArray(0);
	
	return make_pair(hArrayBuffer, hVertexArrayObject);
}

void DisplayTriangles(
	GLuint i_hArrayBuffer,
	GLuint i_hVertexArrayObject,
	GLuint i_nTriangleCount,
	const GLfloat * i_vertices) //%Type here may vary by vertexattribute
{
	glClear(GL_COLOR_BUFFER_BIT); //%Should probably be moved to beginning of draw sequence

	//set array buffer as active, copy 'vertices' into buffer
	glBindBuffer(GL_ARRAY_BUFFER, i_hArrayBuffer);
	glBindVertexArray(i_hVertexArrayObject);

	unsigned int nVertexCount = 3U * i_nTriangleCount;
	unsigned int nComponentCount = 2U * nVertexCount;
	glBufferData( //%don't do this every time
		GL_ARRAY_BUFFER, 
		sizeof(*i_vertices) * nComponentCount, 
		i_vertices, 
		GL_STATIC_DRAW);
	glDrawArrays(
		GL_TRIANGLES,
		0, //Start index
		nComponentCount); //%last argument should be i_nTriangleCount?

	glFlush(); //%not sure what would happen if this were removed. move to end of sequence
}

void DrawPhysicalObject( //%member of physical object?
	GLuint i_hArrayBuffer, //%wrap these two into a struct
	GLuint i_hVertexArrayObject,
	const PhysicalObject & i_oObject)
{
	assert(!(i_oObject.Model().size() % 9U));
	size_t nTriangleCount = i_oObject.Model().size() / 9U;
	
	DisplayTriangles(
		i_hArrayBuffer,
		i_hVertexArrayObject,
		nTriangleCount,
		i_oObject.Model().data());
}

void GameUpdateLoop(
	const Scene & i_oScene)
{
	//Setup timers
	auto nUpdateInterval = chrono::milliseconds(16);
	chrono::steady_clock::time_point nCurrentTime = chrono::steady_clock::now();
	chrono::steady_clock::time_point nNextUpdateTime = nCurrentTime	+ nUpdateInterval;
	
	while(true)
	{
		//Wait until appropriate time.
		while(nCurrentTime < nNextUpdateTime);
		{
			nCurrentTime = chrono::steady_clock::now();
		}
		
		chrono::steady_clock::time_duration nTimeSinceLastUpdate
			= nCurrentTime - (nNextUpdateTime - nUpdateInterval);
		nNextUpdateTime = nCurrentTime + nUpdateInterval;
		//Are we sure we don't want += nUpdateInterval, with the option to 
		//Skip updates if we fall behind. That is, consume events except most recent.
		//Right now we are effectively specifying "at least 16 milliseconds update interval"
		
		double dTimeSinceLastUpdate = 0.001 * static_cast<double>(nTimeSinceLastUpdate); //%convert properly
		assert(dTimeSinceLastUpdate > 0.0);
		//%could store time since last update inside physics objects.
		//%that way we can skip events without accumulating a deltaT
		
		//Input devices update
		//Consuming this information: Update camera position/frame
		
		//Physics tick
		i_oScene.AdvanceStates(dTimeSinceLastUpdate);
		
		//Graphics update
		i_oScene.DrawAll();
	}
}