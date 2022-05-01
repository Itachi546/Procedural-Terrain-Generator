#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H

#include "math_helper.h"
#include <vector>

class GLDebugDraw
{
public:
	GLDebugDraw() {}

	static void addLine(glm::vec3 start, glm::vec3 end) {
		points_.push_back(start);
		points_.push_back(end);
	}

	static void addAABB(const glm::vec3& min, const glm::vec3& max);

	static void draw(float* P, float* V);
private:
	static std::vector<glm::vec3> points_;
};

#endif