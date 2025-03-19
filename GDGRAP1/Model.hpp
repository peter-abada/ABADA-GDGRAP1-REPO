#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Model {
private:

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

public:

    /*

    Model class constructor;
    Creates a model at any given position with the specified attributes

    */
    Model(glm::vec3 pos);

    /*

    Drawing 3D models is now a method of the Model class
    Similar to previously discussed implementation;
    Draws the model by applying transformation matrices and draws from the VAO data

    */
    void draw(GLuint shaderProg, GLuint VAO, std::vector<GLuint>& mesh_indices);

    // Not needed but like why not lol
    glm::vec3 getPosition();
    void setPosition(glm::vec3& pos);

    glm::vec3 getRotation();
    void setRotation(glm::vec3& rot);

    glm::vec3 getScale();
    void setScale(glm::vec3& scl);
};

#endif