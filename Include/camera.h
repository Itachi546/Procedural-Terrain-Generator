#ifndef CAMERA_H
#define CAMERA_H

#include "math_helper.h"

#include <memory>

/*****************************************************************************************************************************************/

class Camera {

public:

	Camera() {}

	virtual glm::vec3 getPosition() const = 0;

	virtual glm::mat4 getProjectionMatrix() const = 0;

	virtual glm::mat4 getViewMatrix() const = 0;

	virtual float getFOV() const = 0;

	virtual float getAspectRatio() const = 0;

	virtual float getZNear() const = 0;

	virtual float getZFar() const = 0;

	virtual glm::vec3 getForward() const = 0;

	virtual glm::vec3 getRight() const = 0;

	virtual glm::vec3 getUp() const = 0;

	virtual std::shared_ptr<Frustum> getFrustum() const = 0;

	virtual ~Camera() {}
};

/*****************************************************************************************************************************************/

class FirstPersonCamera : public Camera
{

public:

	FirstPersonCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);

	glm::vec3 getPosition()         const override { return position_; }

	glm::mat4 getProjectionMatrix() const override { return projectionMatrix_; }

	glm::mat4 getViewMatrix()       const override { return viewMatrix_; }

	float getFOV()                  const override { return fov_; }

	float getAspectRatio()          const override { return aspect_; }

	float getZNear()                const override { return zNear_; }

	float getZFar()                 const override { return zFar_; }

	glm::vec3 getForward()          const override { return forward_; }

	glm::vec3 getRight()            const override { return right_; }

	glm::vec3 getUp()               const override { return up_; }

	void setFOV(float fov) { fov_ = fov; }

	void setAspect(float aspect) { aspect_ = aspect; }

	void setZNear(float zNear) { zNear_ = zNear; }

	void setZFar(float zFar) { zFar_ = zFar; }

	void update(float dt);

	std::shared_ptr<Frustum> getFrustum() const override { return frustum_; }

private:
	void generateProjectionMatrix();

private:
	glm::mat4        projectionMatrix_;
	glm::mat4        viewMatrix_;

	glm::vec3        position_;
	glm::vec3        orientation_;

	glm::vec3        up_;
	glm::vec3        right_;
	glm::vec3        forward_;
	glm::vec3        target_;

	glm::vec3        velocity_ = glm::vec3(0.0f);
	glm::vec3        angularVelocity_ = glm::vec3(0.0f);

	float            cameraSpeed_ = 1.0f;
	float            speedMultiplier_ = 10.0f;
	float            linearDampingFactor_ = 0.03f;
	float            angularDampingFactor_ = 0.03f;
	float            mouseSensitivity_ = 0.01f;


	// Projection Properties
	float            fov_ = glm::radians(70.0f);
	float            aspect_ = 1.333f;
	float            zNear_ = 0.1f;
	float            zFar_ = 1000.0f;

	std::shared_ptr<Frustum> frustum_;
};

#endif