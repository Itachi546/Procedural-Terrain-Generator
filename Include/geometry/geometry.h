#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "vertex_data.h"

class GeometryGenerator
{
public:

	static void GenerateGrid(glm::ivec2 vertexCount, float unitSize, MeshData& outMeshData);

	static void GenerateLTrim(glm::ivec2 vertexCount, float unitSize, MeshData& meshData);
};

#endif
