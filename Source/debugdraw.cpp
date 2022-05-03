#include "debugdraw.h"

#include <glad/glad.h>

std::vector<glm::vec3> GLDebugDraw::points_;

void GLDebugDraw::addAABB(const glm::vec3& min, const glm::vec3& max)
{
	glm::vec3 v0 = min;
	glm::vec3 v1 = glm::vec3(max.x, min.y, min.z);
	glm::vec3 v2 = glm::vec3(max.x, min.y, max.z);
	glm::vec3 v3 = glm::vec3(min.x, min.y, max.z);

	glm::vec3 v4 = glm::vec3(min.x, max.y, min.z);
	glm::vec3 v5 = glm::vec3(max.x, max.y, min.z);
	glm::vec3 v6 = max;
	glm::vec3 v7 = glm::vec3(min.x, max.y, max.z);

	points_.push_back(v0), points_.push_back(v1);
	points_.push_back(v1), points_.push_back(v2);
	points_.push_back(v2), points_.push_back(v3);
	points_.push_back(v3), points_.push_back(v0);

	points_.push_back(v4), points_.push_back(v5);
	points_.push_back(v5), points_.push_back(v6);
	points_.push_back(v6), points_.push_back(v7);
	points_.push_back(v7), points_.push_back(v4);

	points_.push_back(v0), points_.push_back(v4);
	points_.push_back(v1), points_.push_back(v5);
	points_.push_back(v2), points_.push_back(v6);
	points_.push_back(v3), points_.push_back(v7);
}


void GLDebugDraw::addFrustum(glm::vec3* points)
{
	glm::vec3 ntl = points[0];
	glm::vec3 ntr = points[1];
	glm::vec3 nbl = points[2];
	glm::vec3 nbr = points[3];

	glm::vec3 ftl = points[4];
	glm::vec3 ftr = points[5];
	glm::vec3 fbl = points[6];
	glm::vec3 fbr = points[7];

	points_.push_back(nbl), points_.push_back(nbr);
	points_.push_back(nbr), points_.push_back(fbr);
	points_.push_back(fbr), points_.push_back(fbl);
	points_.push_back(fbl), points_.push_back(nbl);

	points_.push_back(ntl), points_.push_back(ntr);
	points_.push_back(ntr), points_.push_back(ftr);
	points_.push_back(ftr), points_.push_back(ftl);
	points_.push_back(ftl), points_.push_back(ntl);


	points_.push_back(ntl), points_.push_back(nbl);
	points_.push_back(ntr), points_.push_back(nbr);
	points_.push_back(ftr), points_.push_back(fbr);
	points_.push_back(ftl), points_.push_back(fbl);
}

/*****************************************************************************************************************************************/

void GLDebugDraw::draw(float* P, float* V)
{
	glUseProgram(0);
	if (points_.size() == 0)
		return;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(P);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(V);

	glColor3f(0.66f, 0.1f, 0.1f);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < points_.size(); i += 2)
	{
		glVertex3f(points_[i].x, points_[i].y, points_[i].z);
		glVertex3f(points_[i + 1].x, points_[i + 1].y, points_[i + 1].z);
	}
	glEnd();
	glLineWidth(1.0f);
	points_.clear();
}

