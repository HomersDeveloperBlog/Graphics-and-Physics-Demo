//An instance of an drawable object.

class Model
{
public:
	Model(const OpenGLBuffer & i_hBuffer,
        c_matrix<double, 3U, 3U> i_aadModelToWorld)
		: m_hBuffer(i_hBuffer),
        m_aadModelToWorld(i_aadModelToWorld) {}
	
	//The model to world matrix for the initial scaling about arbitrary frame
	c_matrix<double, 3U, 3U> ScalingMatrix() const
	{
		return m_aadScaling;
	}
	
private:
	OpenGLBuffer m_hBuffer;
	c_matrix<double, 3U, 3U> m_aadScaling;
};