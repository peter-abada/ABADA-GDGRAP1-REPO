#include "Light.hpp"

Light::Light(glm::vec3 pos, glm::vec3 color) : lightPos(pos), lightColor(color) {}

glm::vec3 Light::getLightPos() {
	return lightPos;
}

void Light::setLightPos(glm::vec3 pos) {
	lightPos = pos;
}

glm::vec3 Light::getLightColor() {
	return lightColor;
}

void Light::setLightColor(glm::vec3 color) {
	lightColor = color;
}