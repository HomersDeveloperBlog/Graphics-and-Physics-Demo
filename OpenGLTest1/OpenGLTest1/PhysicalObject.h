class PhysicalObject
{
public:
	PhysicalObject(
		const double i_dMass,
		const c_matrix<double, 3U, 3U> & i_aadInertia,
		const c_vector<double, 3U> & i_adPosition,
		const c_vector<double, 3U> & i_adVelocity,
		const c_matrix<double, 3U, 3U> & i_adAngularPosition,
		const c_vector<double, 3U> & i_adAngularVelocity)
		: m_oIntrinsicState(i_dMass, i_aadInertia),
		m_oExtrinsicState(i_adPosition, i_adVelocity, i_adAngularPosition, i_adAngularVelocity)
	{
		assert(CheckState());
	}

	bool CheckState() const
	{
		return m_oIntrinsicState.CheckState() 
			&& m_oExtrinsicState.CheckState();
	}

	//%should pass this job to physics object.
	void AdvanceState(const double i_dDT)
	{
		runge_kutta4<c_vector<double, 3U>> stepper; //%An adaptive integrator with non-uniform time-step would be best.
		stepper.do_step(
			m_oIntrinsicState, //.operator(),
			m_oExtrinsicState, //State info to evolve.
			0, //current time. %Could store last update? Have AdvanceState input provide current time?
			i_dDT);
			
		//No guarantee R-matrix will stay orthogonal.
		//Compute the polar decomposition of the matrix.
		//Iterative Heron's method U_k+1 = 0.5 * (U_k + U_k^H^-1) ~= 0.5*(U_k + U_k^T). 4 iterations should do. 
		//Matrix normalization U = A / sqrt(A^H * A). Quite painful.
		
		//Energies may also diverge even if no work is being done.
		//Select an integrator with an energy preserving property.
	}
	
	std::tuple<
		c_vector<double, 3U>, 
		c_matrix<double, 3U, 3U>>
	EvalModelToWorldMatrix() const
	{
		//Matrix is angular position matrix * initial scaling matrix
		c_matrix<double, 3U, 3U> aadModelToWorld = 
			m_oExtrinsicState.m_adAngularPosition * m_oMesh.m_aadScaling;
		
		//May have to assemble into some wierd 4x4
		//Which may need to be converted to OGL compatible format
		
		return make_tuple(
			m_oExtrinsicState.m_adPosition,
			aadModelToWorld);
	}
	
	const OpenGLBuffer & ModelBuffer() const
	{
		return m_oMesh.Buffer();
	}
	
protected:
	PhysicalIntrinsicState m_oIntrinsicState; //mechanical decorator
	PhysicalExtrinsicState m_oExtrinsicState; //position + orientation. also needed for display/collision.
	//ConvexPolyhedron m_oConvexPolyhedron; //collision geometry. also world space
	Model m_oMesh; //display geometry.
};