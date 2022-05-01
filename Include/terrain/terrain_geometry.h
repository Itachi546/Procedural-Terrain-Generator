#ifndef TERRAIN_GEOMETRY_H
#define TERRAIN_GEOMETRY_H


#include "math_helper.h"
#include "terrain_params.h"

#include <vector>
#include <memory>

class GLMesh;
class GLBuffer;
class Camera;

class TerrainGeometry
{
public:

	explicit TerrainGeometry(TerrainParams* params);

	void update(Camera* camera);

	void draw();

private:

	void generateFootprintGeometry(int vertexCount, float unitSize);

	void generateLocations(const glm::vec3& cameraPosition);

	void generateLocationFor(int clipLevel, const glm::vec3& cameraPosition);

	void updateDrawCommands();

	std::shared_ptr<GLMesh> mesh_;

	int m_;
	TerrainParams* params_;


	struct TerrainData
	{
		glm::vec2 translate;
		glm::vec2 scale;
		glm::vec2 id;

		BoundingBox generateBoundingBox();
	};

	std::vector<TerrainData> transformData_;
	std::shared_ptr<GLBuffer> transformBuffer_;
};

#endif
