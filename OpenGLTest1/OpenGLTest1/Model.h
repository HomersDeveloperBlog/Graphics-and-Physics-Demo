//An instance of an drawable object.

#pragma once

#include "PhysicsBase.h"
#include "GLBase.h"
#include "Buffer.h"

class DisplayModel
{
public:
	DisplayModel(const OpenGLBuffer & i_hBuffer,
		const size_t i_nVertexCount, //%ultimately we need buffer slot info. offset and length (in # of vertices)
        const boost::numeric::ublas::c_matrix<double, 3U, 3U> & i_aadScaling)
		: m_hBuffer(i_hBuffer),
		m_nVertexCount(i_nVertexCount),
        m_aadScaling(i_aadScaling) {}

	boost::numeric::ublas::c_matrix<double, 3U, 3U> ScalingMatrix() const {return m_aadScaling;}
	const OpenGLBuffer & Buffer() const {return m_hBuffer;}
	size_t VertexCount() const {return m_nVertexCount;}

private:
	OpenGLBuffer m_hBuffer;
	size_t m_nVertexCount;
	boost::numeric::ublas::c_matrix<double, 3U, 3U> m_aadScaling;
};