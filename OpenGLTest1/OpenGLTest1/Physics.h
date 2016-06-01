#pragma once

//%change hungarian notation for vectors and matrices. vMyVector, tMyTensor. or matMyMatrix;
//%Checkstate should be virtual function of inherited interface : VerifiableState;

#include <array>
//using namespace std;

//ublas references.
//http://www.boost.org/doc/libs/1_60_0/libs/numeric/ublas/doc/index.html
//http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?Effective_UBLAS
//http://www.boost.org/doc/libs/1_60_0/libs/numeric/odeint/doc/html/index.html
//http://na-wiki.csc.kth.se/mediawiki/index.php/Using_uBLAS#Expressions_at_work
//http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?Effective_UBLAS

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric;
using namespace boost::numeric::ublas;

void ublasDemo();

#include <boost/numeric/odeint.hpp>

using namespace boost::numeric::odeint;

void lorenz(
	const ublas::c_vector<double , 3U> & x,
	ublas::c_vector<double, 3U> & dxdt,
	double t);
void write_lorenz(const c_vector<double, 3U> & x, const double t);
void odeintDemo();

//This would be difficult to template by dimension, since angular dof != N
//Would consist only of template specializations
//We could theoretically separate angular and positional state.
class PhysicalIntrinsicState
{
public:
	PhysicalIntrinsicState()
		: m_dMass(0.0),
		m_aadInertia(zero_matrix<double>(3U, 3U)) {}

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

		bool bInertiaTensorIsFullRank = true; //%code test
		assert(bInertiaTensorIsFullRank);

		return bMassIsNonZero && bInertiaTensorIsFullRank;
	}

	//%later we will need a way of including a non-constant force.
	//%say, through a further level of indirection (function pointer, arbitrary arguments)
	//%or by making many evolution functions and picking them in the calling function.

	//void operator()(
	//	const c_vector<c_vector<double, 3U>, 2U> & adState,
	//	c_vector<c_vector<double, 3U>, 2U> & adState_Diff,
	//	const double dTime)
	//{
	//	adState_Diff[0] = adState[1]; //x' = v
	//	adState_Diff[1] = 0.0 * (1.0 / m_dMass); //v' = 1/m * F
	//}

	void operator()(
		const c_vector<double, 3U> & adState,
		c_vector<double, 3U> & adState_Diff,
		const double dTime)
	{
		adState_Diff = zero_vector<double>(3U);
	}

//protected:
	double m_dMass; //Should be inverse mass
	c_matrix<double, 3U, 3U> m_aadInertia; //About centre of mass
};

//Should be dimension-templated
class PhysicalExtrinsicState
{
public:
	PhysicalExtrinsicState()
		: m_adPosition(zero_vector<double>(3U)),
		m_adVelocity(zero_vector<double>(3U)) {}

	PhysicalExtrinsicState(
		const c_vector<double, 3U> & i_adPosition,
		const c_vector<double, 3U> & i_adVelocity)
		: m_adPosition(i_adPosition),
		m_adVelocity(i_adVelocity) 
	{
		assert(CheckState());
	}

	bool CheckState() const {return true;}

//protected:
	c_vector<double, 3U> m_adPosition;
	c_vector<double, 3U> m_adVelocity;
	c_vector<double, 3U> m_adAngularPosition; //%I suppose specializations for symmetric objects could reduce dimension
	c_vector<double, 3U> m_adAngularVelocity;
};

class DisplayMesh
{
public:
	//DisplayMesh() : default;

//protected:
	//c_vector<double, 3U> m_adModelToWorldTranslation; //%already taken care of. physics position.
	c_matrix<double, 3U, 3U> m_matModelToWorldAffine; //%already have an arbitrary rotation.
	//%That means this matrix need only be a rotation followed by a scaling.
	//%might be safe to assume we don't need this functionality.
	//%we can define everything in world space for now. Then just worry about how to map to the screen.

	//%the fact that physics and display have different vector types will probably just be settled by a copying.

	typedef std::array<c_vector<double, 3U>, 3U> TTriangle;
	std::vector<TTriangle> m_vectTriangles; //%many object instances could refer to the same world space mesh memory
};

class PhysicalObject
{
public:
	PhysicalObject() = default;

	PhysicalObject(
		const double i_dMass,
		const c_matrix<double, 3U, 3U> & i_aadInertia,
		const c_vector<double, 3U> & i_adPosition,
		const c_vector<double, 3U> & i_adVelocity)
		: m_oIntrinsicState(i_dMass, i_aadInertia),
		m_oExtrinsicState(i_adPosition, i_adVelocity)
	{
		assert(CheckState());
	}

	bool CheckState() const
	{
		return m_oIntrinsicState.CheckState() 
			&& m_oExtrinsicState.CheckState();
	}

	void AdvanceState(const double i_dDT)
	{ //%An adaptive, time sensitive integrator, would be best.
		runge_kutta4<c_vector<double, 3U>> stepper; //%this should be a data member somewhere.
		stepper.do_step(
			m_oIntrinsicState, //%a function object. somewhat opaque in this form.
			m_oExtrinsicState.m_adPosition, //%we need to consolidate this into a single state.
			0, 
			i_dDT);
	}

protected:
	PhysicalIntrinsicState m_oIntrinsicState; //%mechanics information
	PhysicalExtrinsicState m_oExtrinsicState; //%position + orientation. also needed for display/collision.
	//ConvexPolyhedron m_oConvexPolyhedron;//collision geometry
	DisplayMesh m_oWorldSpaceMesh; //display geometry
};