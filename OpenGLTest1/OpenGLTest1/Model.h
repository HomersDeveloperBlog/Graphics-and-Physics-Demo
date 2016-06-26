//An instance of an drawable object.

#include <boost/numeric/ublas/matrix.hpp>

#include "GLBase.h"

#include "Buffer.h"

using namespace boost::numeric;

class Model
{
public:
	Model(const OpenGLBuffer & i_hBuffer,
        ublas::c_matrix<double, 3U, 3U> i_aadModelToWorld)
		: m_hBuffer(i_hBuffer),
        m_aadModelToWorld(i_aadModelToWorld) {}

	ublas::c_matrix<double, 3U, 3U> ScalingMatrix() const {return m_aadScaling;}
	const OpenGLBuffer & Buffer() const {return m_hBuffer;}

private:
	OpenGLBuffer m_hBuffer;
	ublas::c_matrix<double, 3U, 3U> m_aadScaling;
};