#include "Light.hpp"

Light::Light(glm::vec3 color, float intensity) : lightColor(color), intensity(intensity) {}

glm::vec3 Light::getLightColor() {
    return lightColor;
}

void Light::setLightColor(glm::vec3 color) {
    lightColor = color;
}

float Light::getIntensity() {
    return intensity;
}

void Light::setIntensity(float intensity) {
    this->intensity = intensity;
}

void Light::Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Default implementation does nothing
}
