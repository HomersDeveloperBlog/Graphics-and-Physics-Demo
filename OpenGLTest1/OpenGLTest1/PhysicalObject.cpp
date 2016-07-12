#include "PhysicalObject.h"

using namespace std;

PhysicalObject::PhysicalObject(
	const DisplayModel & i_oDisplayModel,
	const PhysicalIntrinsicState & i_oIntrinsicState,
	const PhysicalExtrinsicState & i_oExtrinsicState)
	: //m_oDisplayModel(i_oDisplayModel), //%uncomment
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

//%should pass this job to physics object.
void PhysicalObject::AdvanceState(
	const double i_dDT)
{
	PhysicalExtrinsicState oState(m_oExtrinsicState);
	runge_kutta4<
		PhysicalExtrinsicState, 
		double, 
		PhysicalExtrinsicState, 
		double, 
		vector_space_algebra> stepper;
	stepper.do_step(
		m_oIntrinsicState, //.operator(),
		oState, //State info to evolve.
		0, //current time. %Could store last update? Have AdvanceState input provide current time?
		m_oExtrinsicState,
		i_dDT);
	
	//No guarantee R-matrix will stay orthogonal.
	//Compute the polar decomposition of the matrix.
	//Iterative Heron's method U_k+1 = 0.5 * (U_k + U_k^H^-1) ~= 0.5*(U_k + U_k^T). 4 iterations should do. 
	//Matrix normalization U = A / sqrt(A^H * A). Quite painful.
		
	//Energies may also diverge even if no work is being done.
	//Select an integrator with an energy preserving property.

	m_oExtrinsicState = oState;
}
	
tuple<
	c_vector<double, 3U>, 
	c_matrix<double, 3U, 3U>>
PhysicalObject::EvalModelToWorldMatrix() const
{
	//Matrix is angular position matrix * initial scaling matrix
	c_matrix<double, 3U, 3U> aadModelToWorld = 
		m_oExtrinsicState.AngularPosition();// * m_oDisplayModel.ScalingMatrix(); //%uncomment
		
	//May have to assemble into some wierd 4x4
	//Which may need to be converted to OGL compatible format
		
	return make_tuple(
		m_oExtrinsicState.Position(),
		aadModelToWorld);
}