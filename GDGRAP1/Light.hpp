#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <GLFW/glfw3.h>

class Light {
protected:
    glm::vec3 lightColor;

public:
    Light(glm::vec3 color);
    glm::vec3 getLightColor();
    void setLightColor(glm::vec3 color);

    virtual void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif
