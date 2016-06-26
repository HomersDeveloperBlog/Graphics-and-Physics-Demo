void Scene::AddObject(
	const PhysicalObject * i_pNewObject)
{
	vectObjects.push_back(i_pNewObject);
}
	
void Scene::AdvanceStates(
	double i_dDT)
{
	//Compute forces
	

	//Step states forward
	for(PhysicalObject * pObject : vectObjects)
	{
		pObject->AdvanceState(i_dDT);
	}
}

void Scene::DrawAll()
{
	//Clear
	glClear(GL_COLOR_BUFFER_BIT);
	if(GetOpenGLError(__FILE__, __LINE__))
		throw;
		
	//Set world to view (camera) matrix.
	// GLfloat afView[16];
	// oCamera.GetGLViewMatrix(afView);
	//	
	//GLint glnLocation = glGetUniformLocation(
	//	oProgram.RawHandle(), //%we don't have this
	//	"matViewMatrix");
	//assert(glnLocation > 0);
	//if(GetOpenGLError(__FILE__, __LINE__)
	//	|| glnLocation <= 0)
	//	throw;
	//	
	//glUniformMatrix4fv(
	//	glnLocation,
	//	1, //one matrix
	//	GL_TRUE, //yes transpose. c and glsl are opposite.
	//	m_afView); //%has no value.
	//if(GetOpenGLError(__FILE__, __LINE__))
	//	throw;
			
	//Draw objects
	for(PhysicalObject * pObject : vectObjects)
	{
		//Set model to world matrix.
		//GLfloat afModelToWorldMatrix[16];
		//pObject->EvalModelToWorldMatrix(afModelToWorldMatrix);
		//			
		//GLint glnLocation2 = glGetUniformLocation(
		//	oProgram.RawHandle(), //%we don't have this
		//	"matModelToWorldMatrix");
		//assert(glnLocation2 > 0);
		//if(GetOpenGLError(__FILE__, __LINE__)
		//	|| glnLocation2 <= 0)
		//	throw;
		//	
		//glUniformMatrix4fv(
		//	glnLocation2,
		//	1, //one matrix
		//	GL_TRUE, //yes transpose
		//	afModelToWorldMatrix);
		//if(GetOpenGLError(__FILE__, __LINE__))
		//	throw;
			
		//Get model buffer. Query buffer size. Bind it.
		size_t nBufferSize = pObject->ModelBuffer().Size();
		GLint glnBufferSize = static_cast<GLint>(nBufferSize);
		assert(glnBufferSize > 0);
			
		pObject->Model().Buffer().Bind();
			
		//Issue draw call.
		glDrawArrays(
			GL_TRIANGLES,
			0,
			glnBufferSize);
		if(GetOpenGLError(__FILE__, __LINE__))
			throw;
			
		//Flush
		glFlush(); //%after the loop?
		if(GetOpenGLError(__FILE__, __LINE__))
			throw;
	}
}