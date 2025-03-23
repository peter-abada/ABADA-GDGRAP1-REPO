#ifndef DIRLIGHT_HPP
#define DIRLIGHT_HPP

#include "Light.hpp"
#include <glm/glm.hpp>

class DirLight : public Light {
private:
    glm::vec3 lightDir;

public:
    DirLight(glm::vec3 dir, glm::vec3 color);
    glm::vec3 getLightDir();
    void setLightDir(glm::vec3 dir);
};

#endif 
