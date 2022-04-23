#include "terrain/terrain.h"
#include "terrain/terrain_geometry.h"
#include "ogl.h"

/*****************************************************************************************************************************************/

Terrain::Terrain(int vertexCount, int unitSize) :
	terrainParams_{ vertexCount, unitSize, 4, 100.0f, 0.0f }
{
	// Create Geometry
	terrainGeometry_ = std::make_shared<TerrainGeometry>(&terrainParams_);

	// Create Shader
	shader_ = std::make_shared<GLProgram>(GLShader("Assets/Shaders/main.vert"), GLShader("Assets/Shaders/main.frag"));

}

/*****************************************************************************************************************************************/

void Terrain::update(Camera* camera, float dt)
{
	terrainGeometry_->update(camera);
}

/*****************************************************************************************************************************************/

void Terrain::draw()
{
	shader_->useProgram();
	terrainGeometry_->draw();
}

/*****************************************************************************************************************************************/

Terrain::~Terrain()
{
}

/*****************************************************************************************************************************************/