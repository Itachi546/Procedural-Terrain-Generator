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

/*****************************************************************************************************************************************/

struct BoundingBox {
	glm::vec3 min_;
	glm::vec3 max_;

	BoundingBox() = default;

	BoundingBox(const glm::vec3& min, glm::vec3& max) : min_(min), max_(max) {}

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

	glm::vec3 getSize()   const {
		return max_ - min_;
	}

	glm::vec3 getCenter() const {
		return (max_ + min_) * 0.5f;
	}
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