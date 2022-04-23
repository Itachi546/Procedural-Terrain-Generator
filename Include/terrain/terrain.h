#ifndef TERRAIN_H
#define TERRAIN_H

#include "terrain_params.h"
#include <memory>

class Camera;
class GLProgram;
class TerrainGeometry;

class Terrain
{
public:

	explicit Terrain(int vertexCount, int unitSize);

	void update(Camera* camera, float dt);

	void draw();

	~Terrain();

private:
	TerrainParams terrainParams_;

	std::shared_ptr<GLProgram> shader_;
	std::shared_ptr<TerrainGeometry> terrainGeometry_;
};


#endif