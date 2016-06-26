#pragma once

//%declare as extern and then define in cpp.

static const size_t nTriangleCount0 = 2U;
static const float aafMeshVertices0[3 * nTriangleCount0][2] = //6 vertices, 2 components each
{
	{0.0, 0.0}, {0.1, 0.0}, {0.0, 0.1},
	{0.0, 0.1}, {0.1, 0.0}, {0.1, 0.1},
};

static const size_t nTriangleCount1 = 2U;
static const float aafMeshVertices1[3 * nTriangleCount1][3] = //6 vertices, 2 components each
{
	{0.0, 0.0, 0.0}, {0.1, 0.0, 0.0}, {0.0, 0.1, 0.0},
	{0.0, 0.1, 0.0}, {0.1, 0.0, 0.0}, {0.1, 0.1, 0.0},
};

//%can even put this elsewhere
enum MeshID
{
	SQUARE_MESH_2D = 0, SQUARE_MESH_3D,
	MESH_COUNT
};

static const size_t anMeshTriangleCount[MESH_COUNT] = 
{
	nTriangleCount0, 
	nTriangleCount1
};

static const size_t anMeshByteCount[MESH_COUNT] = 
{
	nTriangleCount0 * 3U * 2U * sizeof(float), 
	nTriangleCount1 * 3U * 3U * sizeof(float)
};

static const float * apMeshes[MESH_COUNT] = 
{
	&aafMeshVertices0[0][0], 
	&aafMeshVertices1[0][0]
};