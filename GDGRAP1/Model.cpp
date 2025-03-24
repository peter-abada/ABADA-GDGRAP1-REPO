#include "Model.hpp"

Model::Model(glm::vec3 pos, int id) : position(pos), rotation(0.0f, 0.0f, 0.0f), scale(0.5f, 0.5f, 0.5f), id(id) {}

void Model::draw(GLuint shaderProg, GLuint VAO, std::vector<GLuint>& mesh_indices, std::vector<GLfloat> vertData) {
    glm::mat4 identity_matrix4 = glm::mat4(1.0f);

    // Rotate around the origin
    glm::mat4 rotation_matrix = glm::rotate(identity_matrix4, glm::radians(rotation.x), glm::vec3(1.0f, 0, 0));
    rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation.y), glm::vec3(0, 1.0f, 0));
    rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1.0f));

    // Apply translation and scaling
    glm::mat4 transformation_matrix = glm::translate(rotation_matrix, position);
    transformation_matrix = glm::scale(transformation_matrix, scale);

    unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

    glUseProgram(shaderProg);
    glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, vertData.size() / 8);
}

glm::vec3 Model::getPosition() { return position; }
void Model::setPosition(glm::vec3& pos) { position = pos; }

glm::vec3 Model::getRotation() { return rotation; }
void Model::setRotation(glm::vec3& rot) { rotation = rot; }

glm::vec3 Model::getScale() { return scale; }
void Model::setScale(glm::vec3& scl) { scale = scl; }

int Model::getId() { return id; }

void Model::Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        float rotationSpeed = (id == 0) ? 1.0f : 2.0f; // Faster rotations for the light source object
        switch (key) {
           
        case GLFW_KEY_W:

            rotation.x -= rotationSpeed;

            break;
        case GLFW_KEY_S:

            rotation.x += rotationSpeed;


            break;
        case GLFW_KEY_A:
            rotation.y -= rotationSpeed;
            break;
        case GLFW_KEY_D:
            rotation.y += rotationSpeed;
            break;
        case GLFW_KEY_Q:
            rotation.z += rotationSpeed;
            break;
        case GLFW_KEY_E:
            rotation.z -= rotationSpeed;
            break;
        }
    }
}
