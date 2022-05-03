#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#define GLM_FORCE_XYZW_ONLY

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <cmath>

/*****************************************************************************************************************************************/
#define EPSILON  0.001
#define PI       3.14159265358979323846   // pi
#define PI_2     1.57079632679489661923   // pi/2
#define PI_4     0.785398163397448309616  // pi/4

class Camera;

/*****************************************************************************************************************************************/

struct BoundingBox {
	glm::vec3 min_;
	glm::vec3 max_;

	BoundingBox() = default;

	BoundingBox(const glm::vec3& min, const glm::vec3& max) : min_(min), max_(max) {}

	BoundingBox(const glm::vec3* points, std::size_t numPoints)
	{
		glm::vec3 vmin = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 vmax = glm::vec3(std::numeric_limits<float>::min());

		for (int i = 0; i < numPoints; ++i)
		{
			vmin = glm::min(vmin, points[i]);
			vmax = glm::max(vmax, points[i]);
		}

		min_ = vmin;
		max_ = vmax;
	}

	BoundingBox transform(glm::mat4 transform)
	{
		glm::vec3 vertices[] = {
			min_,
			glm::vec3(max_.x, min_.y, min_.z),
			glm::vec3(max_.x, min_.y, max_.z),
			glm::vec3(min_.x, min_.y, max_.z),
			glm::vec3(min_.x, max_.y, min_.z),
			glm::vec3(max_.x, max_.y, min_.z),
			max_,
			glm::vec3(min_.x, max_.y, max_.z)
		};

		glm::vec3 v = glm::vec3(transform * glm::vec4(vertices[0], 1.0f));
		glm::vec3 min = v;
		glm::vec3 max = v;
		for (int i = 1; i < 8; ++i)
		{
			v = glm::vec3(transform * glm::vec4(vertices[i], 1.0f));
			min = glm::min(v, min);
			max = glm::max(v, max);
		}

		return BoundingBox{min, max};
	}


	glm::vec3 getSize()   const {
		return max_ - min_;
	}

	glm::vec3 getCenter() const {
		return (max_ + min_) * 0.5f;
	}
};

/*****************************************************************************************************************************************/
struct Plane
{
	Plane() { }

	Plane(const glm::vec3& normal, float distance) : normal(normal), distance(distance) {}

	// Point in clockwise order
	// Ax + By + Cz + d = 0
	// where normal = (A, B, C)
	// d = -Ax - By - Cz
    // d = -dot(N, P) where P - point in the plane
	Plane(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
	{
		generateFromPoints(p0, p1, p2);
	}

	void generateFromPoints(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
	{
		glm::vec3 e0 = p1 - p0;
		glm::vec3 e1 = p2 - p0;

		normal = glm::normalize(glm::cross(e1, e0));
		distance = -glm::dot(normal, p0);
	}

	float getDistance(const glm::vec3& p) const
	{
		return glm::dot(p, normal) + distance;
	}

	glm::vec3 normal;
	float distance;
};
/*****************************************************************************************************************************************/

class Frustum {

public:
	Frustum() {}

	void generate(Camera* camera);

	bool intersect(const BoundingBox& boundingBox);

	glm::vec3 frustumPoints_[8] = {};
	Plane frustumPlanes_[6] = {};

	enum PointLocation {
		NTL = 0,
		NTR,
		NBL,
		NBR,

		FTL,
		FTR,
		FBL,
		FBR
	};

	enum PlaneLocation
	{
		Near = 0,
		Far,
		Left,
		Right,
		Top,
		Bottom
	};
};

/*****************************************************************************************************************************************/
namespace MathUtils {

	inline bool Equals(float A, float B)
	{
		float diff = A - B;
		return (diff < EPSILON) && (-diff < EPSILON);
	}
};

/*****************************************************************************************************************************************/

#endif