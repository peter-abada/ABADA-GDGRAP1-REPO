#include "DirLight.hpp"

DirLight::DirLight(glm::vec3 dir, glm::vec3 color) : Light(color), lightDir(dir) {}

glm::vec3 DirLight::getLightDir() {
    return lightDir;
}

void DirLight::setLightDir(glm::vec3 dir) {
    lightDir = dir;
}
