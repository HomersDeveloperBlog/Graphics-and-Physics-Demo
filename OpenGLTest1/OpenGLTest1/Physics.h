#pragma once

#include <array>
//using namespace std; //%naming conflict with ublas

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/odeint.hpp>

using namespace boost::numeric;
using namespace boost::numeric::ublas; //%maybe not a good idea to do this
using namespace boost::numeric::odeint; //%maybe not a good idea to do this

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

	//%odeint probably requires PhysicalExtrinsicState to allow +, *scalar.
	//http://headmyshoulder.github.io/odeint-v2/doc/boost_numeric_odeint/odeint_in_detail/state_types__algebras_and_operations.html
	//We must support addition of state_type's and scalar multiplication by time_type
	void operator()(
		const PhysicalExtrinsicState & i_adState,
		PhysicalExtrinsicState & o_adState_Diff, //%aliasing
		const double i_dTime)
	{
		o_adState_Diff.m_adPosition = i_adState.m_adVelocity;
		o_adState_Diff.m_adVelocity = (1.0 / m_dMass) * zero_vector<double>(3U); //%force here.
		
		//W_skew
		c_matrix<double, 3U, 3U> aadWSkew; //%refactor into = skew(w);
		aadWSkew[0][0] = 0.0;
		aadWSkew[0][1] = (-1.0) * i_adState.m_adAngularPosition[2];
		aadWSkew[0][2] = i_adState.m_adAngularPosition[1];
		aadWSkew[1][0] = i_adState.m_adAngularPosition[2];
		aadWSkew[1][1] = 0.0;
		aadWSkew[1][2] = (-1.0) * i_adState.m_adAngularPosition[0];
		aadWSkew[2][0] = (-1.0) * i_adState.m_adAngularPosition[1];
		aadWSkew[2][1] = i_adState.m_adAngularPosition[0];
		aadWSkew[2][2] = 0.0;
		
		//We have R' = w cross R; The matrix ODE R' = W_skew*R
		//The solution is R(dt) = exp(skew(w)*dt)*R(0)
		o_adState_Diff.m_adAngularPosition = aadWSkew * i_adState.m_adAngularPosition;
		
		//I = R^T I_0 R
		c_matrix<double, 3U, 3U> aadTransformedInertia =  //%transpose multiply would be nice
			trans(i_adState.m_adAngularPosition) * m_aadInertia * i_adState.m_adAngularPosition;
		
		//Tau = dL/dt + w x L
		c_vector<double, 3U> adAngularMomentum = zero_vector<double>(3U) //%torque about centre here
			- aadWSkew * aadTransformedInertia * i_adState.m_adAngularVelocity;
		 
		//L=Iw
		o_adState_Diff.m_adAngularVelocity = inverse(aadTransformedInertia) * adAngularMomentum;
	}

protected:
	double m_dMass; //%Should be inverse mass?
	c_matrix<double, 3U, 3U> m_aadInertia; //About centre of mass, in canonical frame. 
	//Should be I^-1? We need both, so we could store both.
};

class PhysicalExtrinsicState
{
public:
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

	bool CheckState() const 
	{
		bool bRIsOrthonormal = true; //%missing test 
		assert(bRIsOrthonormal);
		
		return true;
	}
	
	friend PhysicalIntrinsicState;
	
protected:
	c_vector<double, 3U> m_adPosition;
	c_vector<double, 3U> m_adVelocity;
	c_matrix<double, 3U, 3U> m_adAngularPosition; //R matrix
	c_vector<double, 3U> m_adAngularVelocity; //w vector
};

class DisplayMesh
{
public:
	DisplayMesh(
		size_t i_nTriangleCount, //%probably an object handle would be better.
		const double * i_aadTriangles,
		double i_dIsotropicScaling,
		const c_vector<double, 3U> & i_adWorldSpacePosition,
		const c_matrix<double, 3U, 3U> & i_aadWorldSpaceFrame)
	{
		m_vectTriangles.reserve(i_nTriangleCount);
		
		for(size_t nTriangle = 0U;
			nTriangle < i_nTriangleCount;
			++nTriangle)
		{
			//Parse the raw array into a std::array<c_vector<double, 3U>, 3U>
			//Transform it using ublas arithmetic
			//Push onto the vector
		}
		
		//%might make sense to include an openGL type conversion layer here.
		//%that is, store as openGL types. Ignore for now. I want to try to match the binary.
	}
	
	//%-_- this architecture will not respond to moving objects
	//%you will have to store the literal length/pointer to data. 
	//%A handle / database scheme is going to be hard at compile time.
	//%I'm thinking an enum with model names, and a file name. We would have to load each mesh into a buffer
	//The buffers would be stored index by the enum into a MeshDatabase singleton.
	
protected:
	//%we don't want to have to have to input/output uniforms so its best to store transformed data here.
	//%alternatively we store only a handle to the model space mesh, and store transformation instructions.
	typedef std::array<c_vector<double, 3U>, 3U> TTriangle;
	std::vector<TTriangle> m_vectTriangles;
};

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
		//Iterative Heron's method U_k+1 = U_k + U_k^H^-1 ~= U_k + U_k^T. 4 iterations should do. 
		//Matrix normalization U = A / sqrt(A^H * A). Quite painful.
		
		//Energies may also diverge even if no work is being done.
		//Select an integrator with an energy preserving property.
	}

protected:
	PhysicalIntrinsicState m_oIntrinsicState; //mechanics information
	PhysicalExtrinsicState m_oExtrinsicState; //position + orientation. also needed for display/collision.
	//ConvexPolyhedron m_oConvexPolyhedron; //collision geometry. also world space
	DisplayMesh m_oWorldSpaceMesh; //display geometry
};