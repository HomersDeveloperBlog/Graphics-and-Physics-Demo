//%declare as extern and then define in cpp.

static const size_t nTriangleCount0 = 2U;
static const double anMeshVertices0[3*nTriangleCount0][2] = //6 vertices, 2 components each
{
	{0.0, 0.0}, {0.1, 0.0}, {0.0, 0.1},
	{0.0, 0.1}, {0.1, 0.0}, {0.1, 0.1},
};

static const size_t nTriangleCount1 = 2U;
static const double anMeshVertices1[3*nTriangleCount1][2] = //6 vertices, 2 components each
{
	{0.0, 0.0}, {0.1, 0.0}, {0.0, 0.1},
	{0.0, 0.1}, {0.1, 0.0}, {0.1, 0.1},
};

//%can even put this elsewhere
enum MeshID
{
	SQUARE_MESH_2D = 0, SQUARE_MESH_3D,
	MESH_COUNT
};

static const double * aoMeshes[MESH_COUNT] = 
{
	&anMeshVertices0[0][0], 
	&anMeshVertices1[0][0]
};