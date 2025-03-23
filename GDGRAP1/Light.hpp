#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Light {

protected:
	glm::vec3 lightPos;
	glm::vec3 lightColor;

public:
	Light(glm::vec3 pos, glm::vec3 color);
	
	glm::vec3 getLightPos();
	void setLightPos(glm::vec3 pos);

	glm::vec3 getLightColor();
	void setLightColor(glm::vec3 color);
};

#endif