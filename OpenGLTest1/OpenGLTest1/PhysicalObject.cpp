#include "PhysicalObject.h"

using namespace std;

PhysicalObject::PhysicalObject(
	const Model & i_oModel,
	const PhysicalIntrinsicState & i_oIntrinsicState,
	const PhysicalExtrinsicState & i_oExtrinsicState)
	: m_oModel(i_oModel),
	m_oIntrinsicState(i_oIntrinsicState),
	m_oExtrinsicState(i_oExtrinsicState)
{
	assert(CheckState());
}

bool PhysicalObject::CheckState() const
{
	return m_oIntrinsicState.CheckState() 
		&& m_oExtrinsicState.CheckState();
}

const Model & PhysicalObject::Model() const 
{
	return m_oModel;
}

//%should pass this job to physics object.
void PhysicalObject::AdvanceState(
	const double i_dDT)
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
	
tuple<
	c_vector<double, 3U>, 
	c_matrix<double, 3U, 3U>>
PhysicalObject::EvalModelToWorldMatrix() const
{
	//Matrix is angular position matrix * initial scaling matrix
	c_matrix<double, 3U, 3U> aadModelToWorld = 
		m_oExtrinsicState.AngularPosition() * m_oModel.ScalingMatrix();
		
	//May have to assemble into some wierd 4x4
	//Which may need to be converted to OGL compatible format
		
	return make_tuple(
		m_oExtrinsicState.Position(),
		aadModelToWorld);
}