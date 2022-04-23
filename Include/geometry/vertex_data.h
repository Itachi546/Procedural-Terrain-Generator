#ifndef VERTEX_DATA_H
#define VERTEX_DATA_H

#include <stdint.h>
#include <vector>

#include "math_helper.h"

/*****************************************************************************************************************************************/

struct Mesh final {
	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;

	uint32_t vertexOffset = 0;
	uint32_t indexOffset = 0;
};

struct AttributeLayout {
	int attributeIndex;
	int sizeInCount;
	int stride;
};

/*****************************************************************************************************************************************/

struct MeshData
{
	uint32_t                        meshCount;
	std::vector<uint32_t>           indices;
	std::vector<float>              vertices;
	std::vector<AttributeLayout>    attributeLayout;
	std::vector<Mesh>               meshes;
	std::vector<BoundingBox>        boundingBox;

	int getTotalStride() const
	{
		int stride = 0;
		for (const auto& attribute : attributeLayout)
			stride += attribute.sizeInCount * sizeof(float);
		return stride;
	}

	int getTotalAttributeCount() const
	{
		int count = 0;
		for (const auto& attribute : attributeLayout)
			count += attribute.sizeInCount;
		return count;
	}
};

/*****************************************************************************************************************************************/

#endif