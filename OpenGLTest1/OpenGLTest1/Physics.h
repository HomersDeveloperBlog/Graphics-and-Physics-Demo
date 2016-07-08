#pragma once

#include "PhysicsBase.h"

//http://www.boost.org/doc/libs/1_58_0/libs/utility/operators.htm
#include <boost/operators.hpp>

class PhysicalExtrinsicState
    : boost::addable< PhysicalExtrinsicState
    , boost::subtractable< PhysicalExtrinsicState
    , boost::dividable2< PhysicalExtrinsicState, double
	, boost::multipliable2< PhysicalExtrinsicState, double > > > >
{
public:
	//Constructors
	PhysicalExtrinsicState()
		: m_adPosition(zero_vector<double>(3U)),
		m_adVelocity(zero_vector<double>(3U)),
		m_adAngularPosition(zero_matrix<double>(3U, 3U)),
		m_adAngularVelocity(zero_vector<double>(3U)) {}

	PhysicalExtrinsicState(
		const c_vector<double, 3U> & i_adPosition,
		const c_vector<double, 3U> & i_adVelocity,
		const c_matrix<double, 3U, 3U> & i_adAngularPosition,
		const c_vector<double, 3U> & i_adAngularVelocity)
		: m_adPosition(i_adPosition),
		m_adVelocity(i_adVelocity),
		m_adAngularPosition(i_adAngularPosition),
		m_adAngularVelocity(i_adAngularVelocity)
	{
		assert(CheckState());
	}

	//Testing
	bool CheckState() const 
	{
		bool bRIsOrthonormal = true; //%missing test 
		assert(bRIsOrthonormal);
		
		return true;
	}
	
	//Accessors
	c_vector<double, 3U> & Position() {return m_adPosition;}
	const c_vector<double, 3U> & Position() const {return m_adPosition;}
	c_vector<double, 3U> & Velocity() {return m_adVelocity;}
	const c_vector<double, 3U> & Velocity() const {return m_adVelocity;}
	c_matrix<double, 3U, 3U> & AngularPosition() {return m_adAngularPosition;}
	const c_matrix<double, 3U, 3U> & AngularPosition() const {return m_adAngularPosition;}
	c_vector<double, 3U> & AngularVelocity() {return m_adAngularVelocity;}
	const c_vector<double, 3U> & AngularVelocity() const {return m_adAngularVelocity;}
	
	//Operators
    PhysicalExtrinsicState operator+=(
		const PhysicalExtrinsicState & i_oState)
	{
		m_adPosition += i_oState.Position();
		m_adVelocity += i_oState.Velocity();
		m_adAngularPosition += i_oState.AngularPosition();
		m_adAngularVelocity += i_oState.AngularVelocity();
	}

    PhysicalExtrinsicState operator-=(
		const PhysicalExtrinsicState & i_oState)
    {
		m_adPosition -= i_oState.Position();
		m_adVelocity -= i_oState.Velocity();
		m_adAngularPosition -= i_oState.AngularPosition();
		m_adAngularVelocity -= i_oState.AngularVelocity();
	}
	
	PhysicalExtrinsicState operator*=(
		double i_dConstant)
    {
		m_adPosition *= i_dConstant;
		m_adVelocity *= i_dConstant;
		m_adAngularPosition *= i_dConstant;
		m_adAngularVelocity *= i_dConstant;
	}
	
	PhysicalExtrinsicState operator/=(
		double i_dConstant)
	{
		m_adPosition /= i_dConstant;
		m_adVelocity /= i_dConstant;
		m_adAngularPosition /= i_dConstant;
		m_adAngularVelocity /= i_dConstant;
	}

protected:
	c_vector<double, 3U> m_adPosition;
	c_vector<double, 3U> m_adVelocity;
	c_matrix<double, 3U, 3U> m_adAngularPosition; //R matrix
	c_vector<double, 3U> m_adAngularVelocity; //w vector
};

c_matrix<double, 3U, 3U> GetSkew(
	const c_vector<double, 3U> & i_adA)
{
	c_matrix<double, 3U, 3U> aadReturnValue;

	aadReturnValue(0, 0) = 0.0;
	aadReturnValue(0, 1) = (-1.0) * i_adA[2];
	aadReturnValue(0, 2) = i_adA[1];

	aadReturnValue(1, 0) = i_adA[2];
	aadReturnValue(1, 1) = 0.0;
	aadReturnValue(1, 2) = (-1.0) * i_adA[0];

	aadReturnValue(2, 0) = (-1.0) * i_adA[1];
	aadReturnValue(2, 1) = i_adA[0];
	aadReturnValue(2, 2) = 0.0;

	return aadReturnValue;
}

//%brilliant! inverse is not directly provided. must LU factor first
//http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?LU_Matrix_Inversion

c_vector<double, 3U> SolveLinearSystem(
	const c_matrix<double, 3U, 3U> & i_aadA,
	c_vector<double, 3U> i_adX) //local copy
{
	permutation_matrix<size_t> aadP(i_aadA.size1()); 
	lu_factorize(i_aadA, aadP);
	lu_substitute(i_aadA, aadP, i_adX);

	return i_adX;
}

//We must support addition of state_type's and scalar multiplication by time_type
//http://headmyshoulder.github.io/odeint-v2/doc/boost_numeric_odeint/odeint_in_detail/state_types__algebras_and_operations.html

class PhysicalIntrinsicState
{
public:
	PhysicalIntrinsicState()
		: m_dMass(0.0),
		m_aadInertia(zero_matrix<double>(3U, 3U)) {} //Invalid state

	PhysicalIntrinsicState(
		const double i_dMass,
		const c_matrix<double, 3U, 3U> & i_aadInertia)
		: m_dMass(i_dMass),
		m_aadInertia(i_aadInertia)
	{
		assert(CheckState());
	}

	bool CheckState() const 
	{
		bool bMassIsNonZero = !!m_dMass;
		assert(bMassIsNonZero);

		bool bInertiaTensorIsFullRank = true; //%missing test
		assert(bInertiaTensorIsFullRank);

		return bMassIsNonZero && bInertiaTensorIsFullRank;
	}

	void operator() (
		const PhysicalExtrinsicState & i_adState,
		PhysicalExtrinsicState & o_adState_Diff, //%aliasing
		const double i_dTime)
	{
		o_adState_Diff.Position() = i_adState.Velocity();
		o_adState_Diff.Velocity() = (1.0 / m_dMass) * zero_vector<double>(3U); //%force here.
		
		//W_skew
		c_matrix<double, 3U, 3U> aadWSkew = GetSkew(
			i_adState.AngularVelocity());
		
		//We have R' = w cross R; The matrix ODE R' = W_skew*R
		o_adState_Diff.AngularPosition() = prod(
			aadWSkew, 
			i_adState.AngularPosition());
		
		//I = R^T I_0 R
		c_matrix<double, 3U, 3U> aadRTransposeI = prod(
				trans(i_adState.AngularPosition()), 
				m_aadInertia);

		c_matrix<double, 3U, 3U> aadTransformedInertia = prod(
			aadRTransposeI, 
			i_adState.AngularPosition());
		
		//Tau = dL/dt + w cross L
		c_matrix<double, 3U, 3U> aadWcrossI = prod( //Expression template temp
			aadWSkew, 
			aadTransformedInertia);

		c_vector<double, 3U> adAngularMomentum = zero_vector<double>(3U) //%torque about centre here
			- prod(aadWcrossI, i_adState.AngularVelocity());

		//L=Iw -> w = (I^-1)*L
		o_adState_Diff.AngularVelocity() = SolveLinearSystem(
			aadTransformedInertia, 
			adAngularMomentum);
	}

protected:
	double m_dMass; //%Should be inverse mass?
	c_matrix<double, 3U, 3U> m_aadInertia; //About centre of mass, in canonical frame. 
	//Should be I^-1? We need both, so we could store both.
};