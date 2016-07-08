#pragma once

#include "Model.h"
#include "Physics.h"

class PhysicalObject
{
public:
	PhysicalObject(
		const Model & i_oModel,
		const PhysicalIntrinsicState & i_oIntrinsicState,
		const PhysicalExtrinsicState & i_oExtrinsicState);
	
	bool CheckState() const;
	const Model & Model() const;
	void AdvanceState(const double i_dDT);
	std::tuple<c_vector<double, 3U>, c_matrix<double, 3U, 3U>> EvalModelToWorldMatrix() const;
	
private:
	Model m_oModel; //display geometry.
	PhysicalIntrinsicState m_oIntrinsicState; //mechanical decorator
	PhysicalExtrinsicState m_oExtrinsicState; //position + orientation. also needed for display/collision.
	//ConvexPolyhedron m_oConvexPolyhedron; //collision geometry. also world space
};