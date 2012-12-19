/*! Camera class implementation
 *
 * @author Guido Schmidt
 * @date 07.12.12
 */

#include "Camera.h"

//! Constructor
Camera::Camera()
{
	position = glm::vec3(0.0f, 0.0f, 1.0f);
	lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
}

//! Destructor
Camera::~Camera()
{
}

//! Gets the camera's positon vector
glm::vec3 Camera::GetPosition(void){ return position; }

//! Gets the camera's look at vector
glm::vec3 Camera::GetLookAt(void){ return lookAt; }

//! Gets the camera's up vector
glm::vec3 Camera::GetUp(void){ return up; }

//! Set the camera's position vector
void Camera::SetPosition(glm::vec3 position){ this->position = position; }

//! Set the camera's look at vector
void Camera::SetLookAt(glm::vec3 lookAt){ this->lookAt = lookAt; }

//! Set the camera's up vector
void Camera::SetUp(glm::vec3 up){ this->up = up; }
