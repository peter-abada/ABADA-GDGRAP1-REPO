#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <glad/glad.h>

class Shader {

public:
    Shader(std::string vert, std::string frag, std::string ID);
    GLuint getProg();
    std::string getID();

private:
    GLuint loadProg(std::string vert, std::string frag);
    GLuint shaderProg;
    std::string id;
};

#endif