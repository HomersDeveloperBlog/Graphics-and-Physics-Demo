//An instance of an drawable object.

#pragma once

#include "PhysicsBase.h"
#include "GLBase.h"
#include "Buffer.h"

class Model
{
public:
	Model(const OpenGLBuffer & i_hBuffer,
        const boost::numeric::ublas::c_matrix<double, 3U, 3U> & i_aadScaling)
		: m_hBuffer(i_hBuffer),
        m_aadScaling(i_aadScaling) {}

	boost::numeric::ublas::c_matrix<double, 3U, 3U> ScalingMatrix() const {return m_aadScaling;}
	const OpenGLBuffer & Buffer() const {return m_hBuffer;}

private:
	OpenGLBuffer m_hBuffer;
	boost::numeric::ublas::c_matrix<double, 3U, 3U> m_aadScaling;
};