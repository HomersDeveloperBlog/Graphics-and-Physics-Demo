#pragma once

#include "Model.h"
#include "Physics.h"

class DisplayModel;

class PhysicalObject
{
public:
	PhysicalObject(
		const DisplayModel & i_oDisplayModel,
		const PhysicalIntrinsicState & i_oIntrinsicState,
		const PhysicalExtrinsicState & i_oExtrinsicState);
	
	bool CheckState() const;
	//const DisplayModel & DisplayModel() const {return m_oDisplayModel;} //%uncomment
	void AdvanceState(const double i_dDT);
	std::tuple<c_vector<double, 3U>, c_matrix<double, 3U, 3U>> EvalModelToWorldMatrix() const;
	
protected:
	//DisplayModel m_oDisplayModel; //display geometry. //%uncomment
	PhysicalIntrinsicState m_oIntrinsicState; //mechanical decorator
	PhysicalExtrinsicState m_oExtrinsicState; //position + orientation. also needed for display/collision.
	//ConvexPolyhedron m_oConvexPolyhedron; //collision geometry. also world space
};