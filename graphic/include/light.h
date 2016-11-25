#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


class Light
{
public:
    Light(const glm::vec3& lightPosition)
    {
    	this->lightPosition = lightPosition;
    }

    inline glm::vec3 GetLightPos() { return lightPosition; }

protected:

private:
    glm::vec3 lightPosition;
    glm::vec3 lightPower;
    glm::vec3 lightDirection;
};

#endif // LIGHT_H