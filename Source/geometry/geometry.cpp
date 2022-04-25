#include "geometry/geometry.h"

/***********************************************************************************************************************************/

void GeometryGenerator::GenerateGrid(glm::ivec2 vertexCount, float unitSize, MeshData& meshData)
{
    int numVertices = vertexCount.y * vertexCount.x;
    unsigned int vertexOffset = static_cast<uint32_t>(meshData.vertices.size());

    std::vector<float>& vertices = meshData.vertices;

    for (int y = 0; y < vertexCount.y; ++y)
    {
        for (int x = 0; x < vertexCount.x; ++x)
        {
            vertices.push_back(static_cast<float>(unitSize * x));
            vertices.push_back(static_cast<float>(unitSize * y));
        }
    }

    int numIndices = (vertexCount.x - 1) * (vertexCount.y - 1) * 6;
    unsigned int indexOffset = static_cast<uint32_t>(meshData.indices.size());

    std::vector<unsigned int>& indices = meshData.indices;

    int indicesCount = 0;
    for (int z = 0; z < vertexCount.y - 1; ++z)
    {
        for (int x = 0; x < vertexCount.x - 1; ++x)
        {

            int i0 = x + z * vertexCount.x;
            int i1 = i0 + 1;
            int i2 = (i0 + vertexCount.x);
            int i3 = (i1 + vertexCount.x);

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i3);

            indices.push_back(i0);
            indices.push_back(i3);
            indices.push_back(i1);
        }
    }

    meshData.meshCount++;

    Mesh mesh;
    mesh.vertexCount = numVertices;
    mesh.indexCount = numIndices;
    mesh.vertexOffset = vertexOffset;
    mesh.indexOffset = indexOffset;
    meshData.meshes.push_back(mesh);
}

/***********************************************************************************************************************************/

void GeometryGenerator::GenerateLTrim(glm::ivec2 vertexCount, float unitSize, MeshData& meshData)
{
    unsigned int vertexOffset = static_cast<uint32_t>(meshData.vertices.size());

    std::vector<float>& vertices = meshData.vertices;

    for (int x = 0; x < vertexCount.x; ++x)
    {
        vertices.push_back(unitSize * x);
        vertices.push_back(0.0f);


        vertices.push_back(unitSize * x);
        vertices.push_back(unitSize);
    }

    float yOffset = unitSize;
    for (int y = 0; y < vertexCount.y; ++y)
    {
        vertices.push_back(0.0f);
        vertices.push_back(yOffset + unitSize * y);

        vertices.push_back(unitSize);
        vertices.push_back(yOffset + unitSize * y);
    }

    unsigned int indexOffset = static_cast<uint32_t>(meshData.indices.size());
    std::vector<unsigned int>& indices = meshData.indices;

    for (int i = 0; i < (vertexCount.x - 1); ++i)
    {
        indices.push_back(i * 2);
        indices.push_back(i * 2 + 1);
        indices.push_back(i * 2 + 3);

        indices.push_back(i * 2);
        indices.push_back(i * 2 + 3);
        indices.push_back(i * 2 + 2);
    }

    int offset = vertexCount.x * 2;
    for (int i = 0; i < (vertexCount.y - 1); ++i)
    {
        indices.push_back(offset + i * 2);
        indices.push_back(offset + i * 2 + 3);
        indices.push_back(offset + i * 2 + 1);

        indices.push_back(offset + i * 2);
        indices.push_back(offset + i * 2 + 2);
        indices.push_back(offset + i * 2 + 3);
    }

    int numIndices = (int)indices.size() - indexOffset;
    int numVertices = ((int)vertices.size() - vertexOffset) / 3;

    meshData.meshCount++;

    Mesh mesh;
    mesh.vertexCount = numVertices;
    mesh.indexCount = numIndices;
    mesh.vertexOffset = vertexOffset;
    mesh.indexOffset = indexOffset;
    meshData.meshes.push_back(mesh);
}

/***********************************************************************************************************************************/
