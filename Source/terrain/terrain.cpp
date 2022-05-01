#include "terrain/terrain.h"
#include "terrain/terrain_geometry.h"
#include "ogl.h"
#include "image_utils.h"

/*****************************************************************************************************************************************/

Terrain::Terrain(int vertexCount, float unitSize) :
	terrainParams_{ vertexCount, unitSize, 8, 200.0f, 0.0f, 0.1f }
//terrainParams_{ vertexCount, unitSize, 8, 10.0f, 0.0f, 0.1f }
{
	// Create Geometry
	terrainGeometry_ = std::make_shared<TerrainGeometry>(&terrainParams_);

	// Create Shader
	shader_ = std::make_shared<GLProgram>(GLShader("Assets/Shaders/main.vert"), GLShader("Assets/Shaders/main.frag"));

	{
		// Load Heightmap
		ImageHeader header = {};
		float* data = ImageUtils::LoadImageFloat("Assets/Textures/heightmap.png", header);
		TextureParams params = {};
		params.width = header.width;
		params.height = header.height;
		params.format = TextureFormat::R16F;
		heightMap_ = std::make_shared<GLTexture>(data, params);
		ImageUtils::FreeImage(data);
	}
	{
		// Load Heightmap
		ImageHeader header = {};
		unsigned char* data = ImageUtils::LoadImage("Assets/Textures/gradient.png", header);
		TextureParams params = {};
		params.width = header.width;
		params.height = header.height;
		params.format = TextureFormat::RGB8;
		params.wrapS = params.wrapT = TextureWrap::ClampToEdge;
		gradientMap_ = std::make_shared<GLTexture>(data, params);
		ImageUtils::FreeImage(data);
	}
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
	shader_->setInt("u_Heightmap", 0);
	shader_->setInt("u_GradientMap", 1);
	shader_->setInt("u_VertexCount", terrainParams_.vertexCount);
	shader_->setFloat("u_TextureDims", 2048.0f);
	shader_->setFloat("u_MaxHeight", terrainParams_.maxHeight);
	shader_->setFloat("u_TransitionRegionWidth", terrainParams_.transitionRegionWidth);
	shader_->setFloat("u_UnitSize", terrainParams_.unitSize);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap_->getHandle());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gradientMap_->getHandle());

	terrainGeometry_->draw();
}

/*****************************************************************************************************************************************/

Terrain::~Terrain()
{
}

/*****************************************************************************************************************************************/