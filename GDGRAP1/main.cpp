#include <iostream>
#include <string>
#include <vector>
#include <ctime>
//
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Model.hpp"
#include "OrthoCamera.hpp"
#include "PersCamera.hpp"
#include "DirLight.hpp"

/*
 * PC02:
 *
 *TODO:
 * - Create base class "Camera" to be inherited by classes "OrthoCamera" and "PerspectiveCamera"
 * - Create base class "Light" to be inherited by classes "Dir(ection)Light" and "PointLight"
 * 
 * - Render two objects: one main object and one to represent a light source
 * - Add a direction light at (4, -5, 0), the second object will serve as a point light
 * - Main object should be textured and shaded and account for lighting
 * - Main object rotation controlled by WS (X axis), AD (Y axis) and QE (Z axis)
 * - Light object also controlled by WASDQE to rotate around main object
 * - Press space to switch control between main object and light object
 * - Control point light intensity using up and down and direction light intensity using left and right
 * - Perspective camera rotating around main object controlled with mouse
 * - Orthographic camera viewing both objects from the top
 * - Switch between cameras using 1 and 2
 * 
 * - We'll need a new model
 * 
 * 
*/

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

/*

Function to load shaders from a file path
Parameters are the file paths for the shaders as strings
Returns the final shader program
Load the shader program in main()

*/
GLuint loadShader(std::string vert, std::string frag) {
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

    return shaderProg;
}

int main(void) {

    float height = 600, width = 600;

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "GDGRAP-1 PC02", NULL, NULL);
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

    //Load shaders then link the shader program
    GLuint shaderProg = loadShader("Shaders/sample.vert", "Shaders/sample.frag");
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
        //Data below is likely to be part of camera class
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
