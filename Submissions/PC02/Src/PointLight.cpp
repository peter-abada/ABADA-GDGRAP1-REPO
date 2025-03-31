#include "PointLight.hpp"
#include <iostream>
PointLight::PointLight(glm::vec3 pos, glm::vec3 color, float constant, float linear, float quadratic, float intensity)
    : Light(color, intensity), position(pos), rotation(0.0f), constant(constant), linear(linear), quadratic(quadratic) {
}

glm::vec3 PointLight::getPosition() {
    return position;
}

void PointLight::setPosition(glm::vec3 pos) {
    position = pos;
}

glm::vec3 PointLight::getRotation() {
    return rotation;
}

void PointLight::setRotation(glm::vec3 rot) {
    rotation = rot;
}

float PointLight::getConstant() {
    return constant;
}

void PointLight::setConstant(float constant) {
    this->constant = constant;
}

float PointLight::getLinear() {
    return linear;
}

void PointLight::setLinear(float linear) {
    this->linear = linear;
}

float PointLight::getQuadratic() {
    return quadratic;
}

void PointLight::setQuadratic(float quadratic) {
    this->quadratic = quadratic;
}

/*
    Key_Callback for PointLight, manipulates light intensity    
*/
void PointLight::Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_UP:
            setIntensity(getIntensity() + 1.f);
			//std::cout << "Intensity: " << getIntensity() << std::endl;
            break;
        case GLFW_KEY_DOWN:
            setIntensity(getIntensity() - 0.1f);
            break;
        }
    }
}
