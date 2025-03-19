#include "Model.hpp"

Model::Model(glm::vec3 pos) : position(pos), rotation(0.0f, 0.0f, 0.0f), scale(0.01f, 0.01f, 0.01f) {}

void Model::draw(GLuint shaderProg, GLuint VAO, std::vector<GLuint>& mesh_indices) {
	glm::mat4 identity_matrix4 = glm::mat4(1.0f);
	glm::mat4 transformation_matrix = glm::translate(identity_matrix4, position);
	transformation_matrix = glm::scale(transformation_matrix, scale);
	transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0, 0));
	transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotation.y), glm::vec3(0, 1.0f, 0));
	transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1.0f));
	
	unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));
	
	glUseProgram(shaderProg);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);
}

glm::vec3 Model::getPosition() { return position; }
void Model::setPosition(glm::vec3& pos) { position = pos; }

glm::vec3 Model::getRotation() { return rotation; }
void Model::setRotation(glm::vec3& rot) { rotation = rot; }

glm::vec3 Model::getScale() { return scale; }
void Model::setScale(glm::vec3& scl) { scale = scl; }