#include "camera.h"
#include "input.h"

#include <GLFW/glfw3.h>

/*****************************************************************************************************************************************/

FirstPersonCamera::FirstPersonCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) : 
	position_(position),
	up_(up),
	target_(target),
	orientation_(0.0f)
{
	generateProjectionMatrix();

	frustum_ = std::make_shared<Frustum>();
}

/*****************************************************************************************************************************************/

void FirstPersonCamera::update(float dt)
{
	generateProjectionMatrix();

	float speed = cameraSpeed_;
	if (Input::IsModKeyDown(GLFW_MOD_SHIFT))
		speed *= speedMultiplier_;

	if (Input::IsKeyDown(GLFW_KEY_W))
		velocity_ += forward_ * speed;
	else if (Input::IsKeyDown(GLFW_KEY_S))
		velocity_ -= forward_ * speed;

	if (Input::IsKeyDown(GLFW_KEY_1))
		velocity_ += up_ * speed;
	else if (Input::IsKeyDown(GLFW_KEY_2))
		velocity_ -= up_ * speed;

	if (Input::IsKeyDown(GLFW_KEY_A))
		velocity_ -= right_ * speed;
	else if (Input::IsKeyDown(GLFW_KEY_D))
		velocity_ += right_ * speed;

	velocity_ = (velocity_ - linearDampingFactor_ * velocity_);
	position_ += velocity_ * dt;

	if (Input::IsKeyDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		auto mouseDelta = MouseState::GetMouseDelta();
		angularVelocity_ += glm::vec3(-std::get<1>(mouseDelta) * mouseSensitivity_, -std::get<0>(mouseDelta) * mouseSensitivity_, 0.0f);

	}
	angularVelocity_ = angularVelocity_ - angularDampingFactor_ * angularVelocity_;
	orientation_ += angularVelocity_ * dt;
	static const float halfPI = static_cast<float>(PI_2);
	orientation_.x = glm::clamp(orientation_.x, -halfPI, halfPI);

	glm::mat3 rotationMatrix = glm::yawPitchRoll(orientation_.y, orientation_.x, orientation_.z);
	up_ = glm::normalize(rotationMatrix * glm::vec3(0.0f, 1.0f, 0.0f));
	forward_ = glm::normalize(rotationMatrix * target_);
	right_ = glm::normalize(glm::cross(forward_, up_));
	viewMatrix_ = glm::lookAt(position_, position_ + forward_, up_);

	frustum_->generate(this);

}

/*****************************************************************************************************************************************/

void FirstPersonCamera::generateProjectionMatrix()
{
	projectionMatrix_ = glm::perspective(fov_, aspect_, zNear_, zFar_);
}

/*****************************************************************************************************************************************/