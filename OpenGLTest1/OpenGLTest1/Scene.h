#pragma once

#include "GLBase.h"

//We won't try to separate objects that require physics and objects that require drawing. 
//We also won't try to determine what does and does not need drawing.

class Scene
{
	//Scene()	{}
	
	void AddObject(const PhysicalObject * i_pNewObject);
	void AdvanceStates(double i_dDT);
	void DrawAll();

protected:
	vector<PhysicalObject *> vectObjects; //%not a raw pointer please
	GLfloat m_afCamera[16];
	//uniforms
	//map modelID -> buffer. For now we set buffers directly in main.
};

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