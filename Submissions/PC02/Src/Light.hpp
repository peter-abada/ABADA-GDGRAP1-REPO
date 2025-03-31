#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <GLFW/glfw3.h>

class Light {
protected:
    glm::vec3 lightColor;
    float intensity;

public:
    Light(glm::vec3 color, float intensity = 1.0f);
    glm::vec3 getLightColor();
    void setLightColor(glm::vec3 color);
    float getIntensity();
    void setIntensity(float intensity);

    virtual void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif
