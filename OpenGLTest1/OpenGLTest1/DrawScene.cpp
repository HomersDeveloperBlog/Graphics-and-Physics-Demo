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

//Arrays for integer handles to opengl objects
enum Attrib_IDs { vPosition = 0 }; //An enum for the variable names/indices of the vertex shader.

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
		vPosition, //layout number
		nVectorDimension, //as it is packed in the buffer.
		GL_FLOAT, //buffer type (converted from this type to foat)
		GL_FALSE, //normalize?
		nVectorStride,
		(void*)(0)); //offset from start of bound GL_ARRAY_BUFFER
	glEnableVertexAttribArray(vPosition);
	
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
	//%declare a variable in shader qualified by 'uniform'
	//%I believe all shaders in a linked program have a common namespace
	//To load a value in, we must request its location from its lexical name. 
	// GLint glGetUniformLocation(GLuint program, const char* name);
	//Could return -1 if invalid. Will not change in program.
	//Then set the value using
	// void glUniform{1234}{fdi ui}(GLint location, TYPE value);
	// void glUniform{1234}{fdi ui}v(GLint location, GLsizei count, const TYPE * values);
	// void glUniformMatrix{234}{fd}v(GLint location, GLsizei count, GLboolean transpose, const GLfloat * values);
	//Transpose should be true for C-like arrays. GLSL is column major internal, C is row major.
	//The count allows for the possibility of many such matrices, stored serially.
	
	//%types should probably be converted to GL_FLOAT
	//%otherwise change shaders to accept dvect4, ect.
	//%and the vertexattributepointer call must be adjusted.
	//%might work anyway, if regular v.a.p. function is called, double is automatically downconverted to float.
	//%see vertex specification section.
	
	//%view matrix rows consist of view frame bases.
	//%Multiplication by R^T gives camera basis coordinates from world.
	// https://en.wikipedia.org/wiki/Camera_matrix
	//Translate by -r, rotate by R^t.
	//Rotation matrix is mat4= (R, 0), (0, 1).
	//Translation is mat4 = (0, -r), (0, 1)
	//Homogenous matrices are combined to form projection.
	//Viewing still requires scaling onto normalized device coordinates.
	//Doing so implies clipping outside [-1,1]^2, and amounts to a zoom. 
	//Off-centre perspective transformation is p229 ogl programming guide.
	//Combine homogenous coordinate matrices Projection * View * Model
	
	assert(!(i_oObject.ModelData().size() % 9U));
	size_t nTriangleCount = i_oObject.ModelData().size() / 9U;
	
	DisplayTriangles(
		i_hArrayBuffer,
		i_hVertexArrayObject,
		nTriangleCount,
		i_oObject.ModelData().data());
}

class Scene
{
	//Scene()	{}
	
	//We won't try to separate objects that require physics and objects that require drawing. 
	//We also won't try to determine what does and does not need drawing.
	
	void AddObject(const PhysicalObject * i_pNewObject)
	{
		vectObjects.push_back(i_pNewObject);
	}
	
	void AdvanceStates(double i_dDT)
	{
		//Compute forces
		
		//Step states forward
		for(PhysicalObject * pObject : vectObjects)
		{
			pObject->AdvanceState(i_dT);
		}
	}
	
	void DrawAll()
	{
		//Clear
		glClear(GL_COLOR_BUFFER_BIT);
		if(GetOpenGLError(__FILE__, __LINE__))
			throw;
		
		//Set world to view (camera) matrix.
		// GLfloat afView[16];
		// oCamera.GetGLViewMatrix(afView);
		
		GLint glnLocation = glGetUniformLocation(
			oProgram.RawHandle(), //%we don't have this
			"matViewMatrix");
		assert(glnLocation > 0);
		if(GetOpenGLError(__FILE__, __LINE__)
			|| glnLocation <= 0)
			throw;
		
		glUniformMatrix4fv(
			glnLocation,
			1, //one matrix
			GL_TRUE, //yes transpose. c and glsl are opposite.
			m_afView); //%has no value.
		if(GetOpenGLError(__FILE__, __LINE__))
			throw;
			
		//Draw objects
		for(PhysicalObject * pObject : vectObjects)
		{
			//Set model to world matrix.
			GLfloat afModelToWorldMatrix[16];
			pObject->EvalModelToWorldMatrix(afModelToWorldMatrix);
					
			GLint glnLocation2 = glGetUniformLocation(
				oProgram.RawHandle(), //%we don't have this
				"matModelToWorldMatrix");
			assert(glnLocation2 > 0);
			if(GetOpenGLError(__FILE__, __LINE__)
				|| glnLocation2 <= 0)
				throw;
			
			glUniformMatrix4fv(
				glnLocation2,
				1, //one matrix
				GL_TRUE, //yes transpose
				afModelToWorldMatrix);
			if(GetOpenGLError(__FILE__, __LINE__))
				throw;
			
			//Get model buffer. Query buffer size. Bind it.
			size_t nBufferSize = pObject->ModelBuffer().Size();
			GLint glnBufferSize = static_cast<GLint>(nBufferSize);
			assert(glnBufferSize > 0);
			
			pObject->ModelBuffer().Bind();
			
			//Issue draw call.
			glDrawArrays(
				GL_TRIANGLES,
				0,
				glnBufferSize);
			if(GetOpenGLError(__FILE__, __LINE__))
				throw;
			
			//Flush
			glFlush();
			if(GetOpenGLError(__FILE__, __LINE__))
				throw;
		}
	}
	
protected:
	vector<PhysicalObject *> vectObjects; //%not a raw pointer
	GLfloat m_afCamera[16];
	//map modelID -> buffer. For now we set buffers directly in main.
};

void GameUpdateLoop()
{
	//Setup shader io
	GLuint hArrayBuffer, hVertexArrayObject; //%wrap into struct
	tie(hArrayBuffer, hVertexArrayObject) = SetupShaderIO();
	//%Should be done the first time each model is loaded.
	
	float fVelocity = 0.01f;
	PhysicalObject oPendulum( //%constructor needs to be updated with display mesh
		4.0,
		identity_matrix<double>(3U),
		zero_vector<double>(3U),
		fVelocity * unit_vector<double>(3U, 0U),
		identity_matrix<double>(3U),
		zero_vector<double>(3U));
		
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
		
		chrono::steady_clock::time_duration nTimeSinceLastUpdate = nCurrentTime - (nNextUpdateTime - nUpdateInterval);
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
		oPendulum.AdvanceState(dTimeSinceLastUpdate);
		
		//Graphics update
		DrawPhysicalObject(
			hArrayBuffer,
			hVertexArrayObject,
			oPendulum);
	}
}