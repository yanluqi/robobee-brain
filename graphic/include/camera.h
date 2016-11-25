#ifndef CAMERA_H
#define CAMERA_H

// External Loader
#include "../include/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
	Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& orientation);
	
	~Camera();

	inline glm::mat4 GetVPMat() { return VPMatrix; }
	inline glm::mat4 GetVMat() { return ViewMatrix; }
	inline glm::mat4 GetPMat() { return ProjectionMatrix; }

protected:

private:
	glm::mat4 ViewMatrix;

	glm::mat4 ProjectionMatrix;

	glm::mat4 VPMatrix;
};

#endif // CAMERA_H