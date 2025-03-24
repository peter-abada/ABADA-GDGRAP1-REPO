#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include "Light.hpp"
#include <glm/glm.hpp>

class PointLight : public Light {
private:
    glm::vec3 position;
    glm::vec3 rotation;
    float constant;
    float linear;
    float quadratic;

public:
    PointLight(glm::vec3 pos, glm::vec3 color, float constant, float linear, float quadratic);

    glm::vec3 getPosition();
    void setPosition(glm::vec3 pos);

    glm::vec3 getRotation();
    void setRotation(glm::vec3 rot);

    float getConstant();
    void setConstant(float constant);

    float getLinear();
    void setLinear(float linear);

    float getQuadratic();
    void setQuadratic(float quadratic);

    void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
};

#endif
