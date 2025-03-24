#include "DirLight.hpp"

DirLight::DirLight(glm::vec3 dir, glm::vec3 color) : Light(color), lightDir(dir) {}

glm::vec3 DirLight::getLightDir() {
    return lightDir;
}

void DirLight::setLightDir(glm::vec3 dir) {
    lightDir = dir;
}

void DirLight::Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_LEFT:
            
            break;
        case GLFW_KEY_RIGHT:
            
            break;
        }
    }
}