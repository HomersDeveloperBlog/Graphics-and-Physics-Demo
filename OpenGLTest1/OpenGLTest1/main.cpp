#include "stdafx.h"
#include "assert.h"
#include <tuple>

#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\freeglut.h"

#include "Physics.h"
#include "ShaderBuild.h"
#include "DrawScene.h"

using namespace std;

int main(
	int argc, 
	char * argv[])
{
	//Initialize OpenGL environment, using glut, glew
	InitializeOpenGLEnvironment();
	
	//Compile and link shaders
	CompileShaders(); //%un-hard-code the input
	
	//Then make a pendulum graphic. Possibly by importing an image.
	//We will need to make a 'scene object' which the two loops will cooperate using.
	//Eventually we need this to be 3D with camera movement
	
	//Possibly some more complicated physics. Constraints.
	//Some user iteraction in the physics would be nice.
	//Use a compute shader for the physics, after its written.
	
	//integrate physical object members into integrator
	//need to map a physical coordinate space with units (world space) to screen space

	//For game engine architecture and ideas, see the unity game engine reference manual
	//put triangular mesh processing and efficient dual-phase collision detection on your resume.
	//Adjust percentages to seem more precise

	GameUpdateLoop();
	
	//glutDisplayFunc(UpdateDisplay);
	glutMainLoop();

	return 0;
}
