#include "terrain/terrain_geometry.h"
#include "geometry/vertex_data.h"
#include "geometry/geometry.h"
#include "camera.h"
#include "ogl.h"

#include "debugdraw.h"

#include <algorithm>


/****************************************************************************************************************************************/

TerrainGeometry::TerrainGeometry(TerrainParams* params) : 
	params_(params),
	m_((params->vertexCount + 1) / 4)
{
	transformBuffer_ = std::make_shared<GLBuffer>(nullptr, static_cast<uint32_t>(sizeof(TerrainData) * 1000), GL_DYNAMIC_STORAGE_BIT);
	generateFootprintGeometry(params->vertexCount, params->unitSize);
	
}

/****************************************************************************************************************************************/

void TerrainGeometry::update(Camera* camera)
{
	glm::vec3 cameraPosition = camera->getPosition();
	generateLocations(cameraPosition);
	updateDrawCommands(camera);
}

/****************************************************************************************************************************************/

void TerrainGeometry::draw()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, transformBuffer_->getHandle());

	mesh_->draw();

	transformData_.clear();
}

/****************************************************************************************************************************************/

void TerrainGeometry::generateFootprintGeometry(int vertexCount, float unitSize)
{

	MeshData meshData = {};

	// Generate all the required grids
	float mSize = (m_ - 1) * unitSize;

	// MxM mesh ID: 0
	GeometryGenerator::GenerateGrid(glm::ivec2(m_), unitSize, meshData);

	// Mx2 Mesh ID : 1
	GeometryGenerator::GenerateGrid(glm::ivec2(m_, 2), unitSize, meshData);

	// (M + 1)x2 Mesh ID: 2
	GeometryGenerator::GenerateGrid(glm::ivec2(m_ + 1, 2), unitSize, meshData);

	// 2xM Mesh ID: 3
	GeometryGenerator::GenerateGrid(glm::ivec2(2, m_), unitSize, meshData);

	// V * (V-1) L-Trim ID:4
	GeometryGenerator::GenerateLTrim(glm::ivec2(vertexCount, vertexCount - 1), unitSize, meshData);


	std::vector<AttributeLayout> attributeLayout = { AttributeLayout{0 , 2, 0}};
	meshData.attributeLayout = attributeLayout;
	mesh_ = std::make_shared<GLMesh>(meshData);
}

/****************************************************************************************************************************************/

void TerrainGeometry::generateLocations(const glm::vec3& cameraPosition)
{ 
	// Generate Location for all clipmap level
	for (int i = 0; i < params_->maxClipLevelCount; ++i)
		generateLocationFor(i, cameraPosition);
}

/****************************************************************************************************************************************/

void TerrainGeometry::updateDrawCommands(Camera* camera)
{
	std::sort(transformData_.begin(), transformData_.end(), [](const TerrainData& a, const TerrainData& b) {
		return a.id.x < b.id.x;
		});

	float currentId = transformData_[0].id.x;
	BoundingBox aabb = mesh_->boundingBoxes_[int(currentId)];
	int totalInstance = 0;

	const auto frustum = camera->getFrustum();
	DrawElementsIndirectCommand* commands = mesh_->commands;
	for (auto& transform : transformData_)
	{

		glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(transform.translate.x, 0.0f, transform.translate.y)) *
			glm::rotate(glm::mat4(1.0f), transform.id.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(transform.scale.x, params_->maxHeight - params_->minHeight, transform.scale.y));

		BoundingBox box_ = aabb.transform(transformMatrix);
		GLDebugDraw::addAABB(box_.min_, box_.max_);

		if (currentId == transform.id.x)
		{
			if (frustum->intersect(box_) || transform.id.x == 4.0f)
			{
				GLDebugDraw::addAABB(box_.min_, box_.max_);
				totalInstance++;
			}
			else
				transform.id = glm::vec2(-1.0f);
		}
		else
		{
			commands[int(currentId)].instanceCount_ = totalInstance;
			currentId = transform.id.x;
			aabb = mesh_->boundingBoxes_[int(currentId)];
			totalInstance = 1;
		}
	}

	transformData_.erase(std::remove_if(
		transformData_.begin(), transformData_.end(),
		[](const TerrainData& data) {
			return data.id.x < 0.0f;
		}), transformData_.end());

	commands[int(currentId)].instanceCount_ = totalInstance;
	glNamedBufferSubData(transformBuffer_->getHandle(), 0, sizeof(TerrainData) * transformData_.size(), transformData_.data());
}

/****************************************************************************************************************************************/

void TerrainGeometry::generateLocationFor(int clipLevel, const glm::vec3& cameraPosition)
{
	glm::vec2 scale = glm::vec2((float)(1 << clipLevel));
	float unitSize = params_->unitSize;
	float gridSize = scale.x * unitSize * (m_ - 1);
	float tileSize = scale.x * unitSize;

	glm::vec2 offset = glm::floor(glm::vec2(cameraPosition.x, cameraPosition.z) / tileSize) * tileSize;

	glm::vec2 tl = glm::vec2{ -gridSize * 2.0f };
	glm::vec2 startPos = tl + offset;

	// Generate all MxM Grid
	for (int y = 0; y < 4; ++y)
	{
		if (y == 2)
			startPos.y += tileSize;

		for (int x = 0; x < 4; ++x)
		{

			if (x == 2)
				startPos.x += tileSize;

			if (clipLevel == 0)
				transformData_.push_back(TerrainData{ startPos, scale, glm::vec2(0.0f, 0.0f) });
			else 
			{
				if ((y == 0 || y == 3) || ((y == 1 || y == 2) && (x == 0 || x == 3)))
					transformData_.push_back(TerrainData{ startPos, scale, glm::vec2(0.0f) });
			}

			startPos.x += gridSize;
		}

		startPos.x = tl.x + offset.x;
		startPos.y += gridSize;
	}

	// Generate CrossHair Y-Direction
	transformData_.push_back(TerrainData{ glm::vec2(0.0f, tl.y) + offset, scale, glm::vec2(3.0f, 0.0f) });
	transformData_.push_back(TerrainData{ glm::vec2(offset.x, startPos.y - gridSize), scale, glm::vec2(3.0f, 0.0f) });

	// Generate CrossHair X-Direction
	transformData_.push_back(TerrainData{ glm::vec2(tl.x, 0.0f) + offset, scale, glm::vec2(1.0f, 0.0f) });
	transformData_.push_back(TerrainData{ glm::vec2(gridSize + tileSize, 0.0f) + offset, scale, glm::vec2(1.0f, 0.0f) });

	if (clipLevel == 0)
	{
		// Generate CrossHair X-Direction
		transformData_.push_back(TerrainData{ glm::vec2(tl.x + gridSize, 0.0f) + offset, scale, glm::vec2(1.0f, 0.0f) });
		transformData_.push_back(TerrainData{ offset, scale, glm::vec2(2.0f, 0.0f) });

		// Generate CrossHair Y-Direction
		transformData_.push_back(TerrainData{ glm::vec2(0.0f, tl.y + gridSize) + offset, scale, glm::vec2(3.0f, 0.0f) });
		transformData_.push_back(TerrainData{ glm::vec2(0.0f, tileSize) + offset, scale, glm::vec2(3.0f, 0.0f) });
	}

	if (clipLevel == params_->maxClipLevelCount - 1)
		return;

	float nextTileSize = 2.0f * tileSize;
	glm::vec2 offset2 = glm::floor(glm::vec2(cameraPosition.x, cameraPosition.z) / nextTileSize) * nextTileSize;

	float dx = (offset2.x - offset.x);
	float dy = (offset2.y - offset.y);
	dx = dx / tileSize;
	dy = dy / tileSize;
	glm::vec2 translate = glm::vec2(0.0f);
	float rotate = 0.0f;

	if (dx == 0.0f && dy == 0.0f) 	// Bottom Right (0, 0)
	{
		translate = glm::vec2(gridSize * 2.0f + tileSize * 2.0f);
		rotate = glm::radians(180.0f);
	}
	else if (dx == 0.0f && dy == -1.0f) // Top Right (0, -1)
	{
		translate = glm::vec2(gridSize * 2.0f + tileSize * 2.0f, -gridSize * 2.0f - tileSize);
		rotate = glm::radians(90.0f);
	}
	else if (dx == -1.0f && dy == -1.0f) // Top Left (-1, -1)
	{
		translate = glm::vec2(-gridSize * 2.0f - tileSize, -gridSize * 2.0f - tileSize);
		rotate = 0.0f;
	}
	else  // Bottom Left (-1, 0)
	{
		translate = glm::vec2(-gridSize * 2.0f - tileSize, gridSize * 2.0f + tileSize * 2.0f);
		rotate = glm::radians(-90.0f);
	}

	transformData_.push_back(TerrainData{ translate + offset, scale, glm::vec2(4.0f, rotate) });
}

/****************************************************************************************************************************************/

BoundingBox TerrainGeometry::TerrainData::generateBoundingBox()
{
	return BoundingBox();
}
