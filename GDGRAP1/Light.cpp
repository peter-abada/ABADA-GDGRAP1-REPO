#include "Light.hpp"

Light::Light(glm::vec3 color) : lightColor(color) {}

glm::vec3 Light::getLightColor() {
    return lightColor;
}

void Light::setLightColor(glm::vec3 color) {
    lightColor = color;
}
