#include "include/camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& orientation)
{
	ViewMatrix = glm::lookAt(
					position,
                    direction, 
                    orientation  
                    );

    ProjectionMatrix = glm::perspective(
                            glm::radians(45.0f),            
                            4.0f / 3.0f,           
                            0.1f,                  
                            100.0f
                            );

    VPMatrix = ProjectionMatrix * ViewMatrix;

}

Camera::~Camera()
{

}