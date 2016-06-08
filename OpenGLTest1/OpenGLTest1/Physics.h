#pragma once

//%this file needs to be split between physics
//%display mesh in ogl core
//%physicalobject in separate file by itself

#include <array>
//using namespace std; //%naming conflict with ublas

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/odeint.hpp>

using namespace boost::numeric;
using namespace boost::numeric::ublas; //%maybe not a good idea to do this
using namespace boost::numeric::odeint; //%maybe not a good idea to do this

//%would it be terrible to have this derive from PhysicalExtrinsicState?
//%PhysicalObject : GalileanBody : NewtonianBody ? or something
//%Would provide Galilean mechanics without mass/inertia.
//%We could even separate velocities. Immovable objects with no_op AdvanceState make sense.
//%in general the different physics are just different advance state.
//%Advance State doesn't have to derive from physics, it just has to update position/orientation
//%An predetermined animation is also possible.
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
		aadWSkew[0][1] = (-1.0) * i_adState.m_adAngularVelocity[2];
		aadWSkew[0][2] = i_adState.m_adAngularVelocity[1];
		aadWSkew[1][0] = i_adState.m_adAngularVelocity[2];
		aadWSkew[1][1] = 0.0;
		aadWSkew[1][2] = (-1.0) * i_adState.m_adAngularVelocity[0];
		aadWSkew[2][0] = (-1.0) * i_adState.m_adAngularVelocity[1];
		aadWSkew[2][1] = i_adState.m_adAngularVelocity[0];
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
	
	const c_vector<double, 3U> & Position() const
	{
		return m_adPosition;
	}
	
	const c_matrix<double, 3U, 3U> & AngularPosition() const
	{
		return m_adAngularPosition;
	}
	
	friend PhysicalIntrinsicState;
	
protected:
	c_vector<double, 3U> m_adPosition;
	c_vector<double, 3U> m_adVelocity;
	c_matrix<double, 3U, 3U> m_adAngularPosition; //R matrix
	c_vector<double, 3U> m_adAngularVelocity; //w vector
};

//%need an internal ogl compatible format here.
//%Must allow for conversion from database format to ogl direct.
//%use a nicer ogl format than a component vector. 
//%then adjust strides in vertex attribute to accommodate new format.
//%model to world currently unused. Implied equivalence.
//%Probably try to store each model in its own OGL buffer.
//%Multiple objects in the same buffer can draw in the same call, but we don't need this.
//%This would not require that we treat vertex attribute and buffer separately,
//%so long as we are willing to make more vertex attributes than we need,
//%and more combine differently formatted data into the same buffer.
class DisplayMesh
{
public:
	DisplayMesh(
		size_t i_nTriangleCount, //%later only a model handle
		const double * i_aadTriangles, //%""
		c_matrix<double, 3U, 3U> i_aadModelToWorld)
		: m_aadModelToWorld(i_aadModelToWorld) //%needs conversion to ogl format.
	{
		size_t nComponentCount = 3U * 3U * i_nTriangleCount;
		m_vectModelSpaceTriangles.reserve(nComponentCount);
		
		for(size_t nComponent = 0U;
			nComponent < nComponentCount;
			++nComponent)
		{
			m_vectModelSpaceTriangles.push_back(
				i_aadTriangles[nComponent]);
		}
		
		//Parse the raw array into internal format.
		// for(size_t nTriangle = 0U;
		// 	nTriangle < i_nTriangleCount;
		// 	++nTriangle)
		// {
		// 	TTriangle3D aadTriangle;
		// 	for(unsigned int nVertex = 0U;
		// 		nVertex < 3U;
		// 		++nVertex)
		// 	{
		// 		aadTriangle[nVertex][] = ;
		// 	}
		// 	//Push onto the vector
		// }
		//%might make sense to include an openGL type conversion layer here.
		//%Ignore for now. I want to try to match the binary.
	}
	
	//The model to world matrix for the initial scaling about arbitrary frame
	//Can't be combined with mechanical position / orientation
	const c_matrix<double, 3U, 3U> & ScalingMatrix() const
	{
		return m_aadScaling;
	} 
	
	//%need some hiding of internal format at some layer.
	const std::vector<double> & ModelData() const
	{
		return m_vectModelSpaceTriangles;
	}
	
protected:
	//typedef std::array<GLdouble, 9U> TOGLTriangle3D;
	std::vector<double> m_vectModelSpaceTriangles; //%later store a pointer to master copy.
	
	//%Alternatively we store only a handle to the model space mesh, and store transformation instructions.
	//size_t m_nModelID; //%index into model database. //%not actually needed once lookup is complete.
	c_matrix<double, 3U, 3U> m_aadScaling; //%should be ogl format?
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
	
	std::tuple<
		c_vector<double, 3U>, 
		c_matrix<double, 3U, 3U>>
	EvalModelToWorldTransform() const
	{
		//Matrix is angular position matrix * initial scaling matrix.AdvanceState
		c_matrix<double, 3U, 3U> aadModelToWorld = 
			m_oExtrinsicState.m_adPosition * m_oWorldSpaceMesh.m_aadScaling;
		
		//May have to assemble into some wierd 4x4
		//Which may need to be converted to OGL compatible format
		
		return make_tuple(
			m_oExtrinsicState.m_adPosition,
			aadModelToWorld);
	}
	
	const std::vector<double> & ModelData() const
	{
		return m_oWorldSpaceMesh.ModelData();
	}
	
protected:
	PhysicalIntrinsicState m_oIntrinsicState; //mechanical decorator
	PhysicalExtrinsicState m_oExtrinsicState; //position + orientation. also needed for display/collision.
	//ConvexPolyhedron m_oConvexPolyhedron; //collision geometry. also world space
	DisplayMesh m_oWorldSpaceMesh; //display geometry
};