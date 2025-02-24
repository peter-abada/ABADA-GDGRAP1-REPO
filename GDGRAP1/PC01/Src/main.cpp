#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*

MODEL CREDIT:
"Low Poly Rat" by snippysnappets
Acquired from https://free3d.com/3d-model/low-poly-rat-3205.html

*/
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
    Model(glm::vec3 pos) : position(pos), rotation(0.0f, 0.0f, 0.0f), scale(0.01f, 0.01f, 0.01f) {}

    /*
    
    Drawing 3D models is now a method of the Model class
    Similar to previously discussed implementation;
    Draws the model by applying transformation matrices and draws from the VAO data

    */
    void draw(GLuint shaderProg, GLuint VAO, std::vector<GLuint>& mesh_indices) {
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

    // Not needed but like why not lol
    glm::vec3 getPosition() { return position; }
    void setPosition(glm::vec3& pos) { position = pos; }

    glm::vec3 getRotation() { return rotation; }
    void setRotation(glm::vec3& rot) { rotation = rot; }

    glm::vec3 getScale() { return scale; }
    void setScale(glm::vec3& scl) { scale = scl; }
};

//Vector that stores an array of Model objects that will be drawn
std::vector<Model> models;

float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 2.0f;
float cameraSpeed = 0.1f;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f; 
float pitch = 0.0f;
float lastX = 300.f, lastY = 300.f;
bool initialMouse = true;


/*

Variables and function for spawn cooldown; Uses <ctime> / <time.h>

Two clock_t variables, one that tracks how much time has passed and a cooldown period

Note that the clock() timer in ctime ALWAYS GOES UP, and cannot be reset

The function checks if the time elapsed is greater than the cooldown (3 seconds * how many times 3 seconds have passed)
If true, set canSpawn to true (let an object be spawned) and add 3 seconds the cooldown

*/

clock_t start = clock();
clock_t cooldown = 3 * CLOCKS_PER_SEC;
bool canSpawn = false;

void spawnCooldown(clock_t start, clock_t& cooldown) {

    clock_t elapse = clock() - start;
    if (elapse > cooldown) {
        cooldown += 3 * CLOCKS_PER_SEC;
        canSpawn = true;
    }
}

//Mouse callback for camera control using mouse
void Mouse_Callback(GLFWwindow* window, double xpos, double ypos) {

	if (initialMouse) { // fixes the mouse going crazy at the start
		lastX = xpos;
		lastY = ypos;
		initialMouse = false;
	}


    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

}

//Key callback for moving the camera and spawning models using the keyboard
void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        glm::vec3 cameraPos = glm::vec3(cameraX, cameraY, cameraZ);
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp)); // for right and leftr movement

        switch (key) {
        //Spawns a new model by pushing a new Model object to the models vector
        case GLFW_KEY_SPACE:
            //Function call for spawn cooldown, a model object can be added if canSpawn == true
            spawnCooldown(start, cooldown);
            if(canSpawn){
                models.push_back(Model(cameraPos + cameraFront)); // Spawn in front of the camera
                canSpawn = false;
            }
            break;
        case GLFW_KEY_W:
            cameraX += cameraSpeed * cameraFront.x;
            cameraZ += cameraSpeed * cameraFront.z;
            break;
        case GLFW_KEY_S:
            cameraX -= cameraSpeed * cameraFront.x;
            cameraZ -= cameraSpeed * cameraFront.z;
            break;
        case GLFW_KEY_A:
            cameraX -= cameraSpeed * cameraRight.x;
            cameraZ -= cameraSpeed * cameraRight.z;
            break;
        case GLFW_KEY_D:
            cameraX += cameraSpeed * cameraRight.x;
            cameraZ += cameraSpeed * cameraRight.z;
            break;
        }
    }
}

int main(void) {

    float height = 600, width = 600;

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "GDGRAP-1 PC01", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    //glViewport(0, 0, 1200, 1200);

    glfwSetKeyCallback(window, Key_Callback);
    glfwSetCursorPosCallback(window, Mouse_Callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();

    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);

    glLinkProgram(shaderProg);

    std::string path = "3D/rat.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(&attributes, &shapes, &material, &warning, &error, path.c_str());

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
    }

    GLfloat vertices[]{
        -0.5f, -0.5f, 0,
        0, 0.5f, 0,
        0.5f, -0.5f, 0
    };

    GLuint indices[] = {
        0,1,2
    };

    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * attributes.vertices.size(),
        &attributes.vertices[0],
        GL_STATIC_DRAW);

    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh_indices.size(), mesh_indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Perspective matrix
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), height / width, 0.1f, 100.0f);

    // Initialize the first model
    models.push_back(Model(glm::vec3(0.0f, 0.0f, 0.0f)));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        //Camera position / eye
        glm::vec3 cameraPos = glm::vec3(cameraX, cameraY, cameraZ);
        glm::mat4 cameraPosMatrix = glm::translate(glm::mat4(1.0f), cameraPos * -1.0f);

        glm::vec3 WorldUp = glm::normalize(glm::vec3(0, 1.0f, 0)); // Pointing up
        glm::vec3 Center = cameraPos + cameraFront; // Camera is looking towards the front

        glm::vec3 Forward = glm::normalize(Center - cameraPos);
        glm::vec3 Right = glm::normalize(glm::cross(Forward, WorldUp));
        glm::vec3 Up = glm::normalize(glm::cross(Right, Forward));

        glm::mat4 cameraOrientation = glm::mat4(1.0f);

        cameraOrientation[0][0] = Right.x;
        cameraOrientation[1][0] = Right.y;
        cameraOrientation[2][0] = Right.z;

        cameraOrientation[0][1] = Up.x;
        cameraOrientation[1][1] = Up.y;
        cameraOrientation[2][1] = Up.z;

        cameraOrientation[0][2] = -Forward.x;
        cameraOrientation[1][2] = -Forward.y;
        cameraOrientation[2][2] = -Forward.z;

        glm::mat4 viewMatrix = cameraOrientation * cameraPosMatrix;

        unsigned int projLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        
        for (Model& model : models) {
            model.draw(shaderProg, VAO, mesh_indices);
        }
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
