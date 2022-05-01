#include "math_helper.h"
#include "camera.h"

/***************************************************************************************************************************/

void Frustum::generate(Camera* camera)
{
	const float tanHFov = std::tan(camera->getFOV() * 0.5f);

	const glm::vec3 position = camera->getPosition();
	const glm::vec3 forward = camera->getForward();
	const float zNear = camera->getZNear();

	// calculate the point in the middle of near plane
	const glm::vec3 nearPoint = position + zNear * forward;

	const float aspect = camera->getAspectRatio();

	// calculate near half width as tan(fov) * zNear_
	const float nhh = tanHFov * zNear;
	const float nhw = aspect * nhh;

	const glm::vec3 right = camera->getRight();
	const glm::vec3 up = camera->getUp();
	// calculate the right and up offset to reach the edge of near plane
	const glm::vec3 nearRight = nhw * right;
	const glm::vec3 nearUp = nhh * up;

	frustumPoints_[PointLocation::NTL] = nearPoint - nearRight + nearUp;
	frustumPoints_[PointLocation::NTR] = nearPoint + nearRight + nearUp;
	frustumPoints_[PointLocation::NBL] = nearPoint - nearRight - nearUp;
	frustumPoints_[PointLocation::NBR] = nearPoint + nearRight - nearUp;

	const float zFar = camera->getZFar();

	// same with the far plane
	const glm::vec3 farPoint = position + zFar * forward;
	const float fhh = tanHFov * zFar;
	const float fhw = aspect * fhh;

	const glm::vec3 farRight = fhw * right;
	const glm::vec3 farUp = fhh * up;

	frustumPoints_[PointLocation::FTL] = farPoint - farRight + farUp;
	frustumPoints_[PointLocation::FTR] = farPoint + farRight + farUp;
	frustumPoints_[PointLocation::FBL] = farPoint - farRight - farUp;
	frustumPoints_[PointLocation::FBR] = farPoint + farRight - farUp;

	/*
	pl[TOP].set3Points(ntr, ntl, ftl);
	pl[BOTTOM].set3Points(nbl, nbr, fbr);
	pl[LEFT].set3Points(ntl, nbl, fbl);
	pl[RIGHT].set3Points(nbr, ntr, fbr);
	pl[NEARP].set3Points(ntl, ntr, nbr);
	pl[FARP].set3Points(ftr, ftl, fbl);
	*/

	// generate plane from the point
	frustumPlanes_[Top].generateFromPoints(frustumPoints_[FTL], frustumPoints_[NTL], frustumPoints_[NTR]);
	frustumPlanes_[Bottom].generateFromPoints(frustumPoints_[FBR], frustumPoints_[NBR], frustumPoints_[NBL]);
	frustumPlanes_[Left].generateFromPoints(frustumPoints_[FBL], frustumPoints_[NBL], frustumPoints_[NTL]);
	frustumPlanes_[Right].generateFromPoints(frustumPoints_[FBR], frustumPoints_[NTR], frustumPoints_[NBR]);
	frustumPlanes_[Near].generateFromPoints(frustumPoints_[NBR], frustumPoints_[NTR], frustumPoints_[NTL]);
	frustumPlanes_[Far].generateFromPoints(frustumPoints_[FBL], frustumPoints_[FTL], frustumPoints_[FTR]);
}

/***************************************************************************************************************************/

void Frustum::intersect(const BoundingBox& boundingBox)
{

}

/***************************************************************************************************************************/