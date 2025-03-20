#include <iostream>
#include <string>
#include <vector>
#include <ctime>
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

// Vector that stores an array of Model objects that will be drawn
std::vector<Model> models;

// Camera and light objects
OrthoCamera orthoCamera(glm::vec3(0.0f, 5.f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
PersCamera persCamera(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
Camera* currentCamera = &persCamera;

float cameraSpeed = 0.1f;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 300.f, lastY = 300.f;
bool initialMouse = true;
bool leftMouseButtonPressed = false;
int selectedModelId = 0; // Add a variable to track the selected model

// Mouse callback for camera control using mouse
void Mouse_Callback(GLFWwindow* window, double xpos, double ypos) {

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        leftMouseButtonPressed = true;
    }
    else {
        leftMouseButtonPressed = false;
        initialMouse = true;
    }

    if (leftMouseButtonPressed && dynamic_cast<PersCamera*>(currentCamera)) {
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

        // Calculate the new camera position to rotate around the main object
        glm::vec3 target = models[0].getPosition();
        glm::vec3 direction = glm::normalize(front);
        float distance = glm::length(currentCamera->getPosition() - target);
        glm::vec3 newPosition = target - direction * distance;

        currentCamera->setPosition(newPosition);
        currentCamera->setFront(glm::normalize(target - newPosition));
    }
}

// Key callback for moving the camera and spawning models using the keyboard
void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        glm::vec3 cameraPos = currentCamera->getPosition();
        glm::vec3 cameraRight = glm::normalize(glm::cross(currentCamera->getFront(), currentCamera->getUp())); // for right and left movement

        switch (key) {
        case GLFW_KEY_SPACE:
            selectedModelId = (selectedModelId == 0) ? 1 : 0; // Swap control between the first and second models
            break;
        case GLFW_KEY_W:
        case GLFW_KEY_S:
        case GLFW_KEY_A:
        case GLFW_KEY_D:
        case GLFW_KEY_Q:
        case GLFW_KEY_E:
            models[selectedModelId].Key_Callback(window, key, scancode, action, mods);
            break;
        case GLFW_KEY_1:
            currentCamera = &orthoCamera;
            break;
        case GLFW_KEY_2:
            currentCamera = &persCamera;
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

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    glfwSetKeyCallback(window, Key_Callback);
    glfwSetCursorPosCallback(window, Mouse_Callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load shaders then link the shader program
    GLuint shaderProg = loadShader("Shaders/sample.vert", "Shaders/sample.frag");
    glLinkProgram(shaderProg);

    // Load the first model 
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

    // Load the second model 
    std::string obj2_path = "3D/rat.obj";
    std::vector<tinyobj::shape_t> obj2_shapes;
    std::vector<tinyobj::material_t> obj2_material;

    tinyobj::attrib_t obj2_attributes;

    success = tinyobj::LoadObj(&obj2_attributes, &obj2_shapes, &obj2_material, &warning, &error, obj2_path.c_str());

    std::vector<GLuint> obj2_mesh_indices;
    for (int i = 0; i < obj2_shapes[0].mesh.indices.size(); i++) {
        obj2_mesh_indices.push_back(obj2_shapes[0].mesh.indices[i].vertex_index);
    }

    GLuint VAOs[2], VBOs[2], EBOs[2];

    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    glGenBuffers(2, EBOs);

    // Setup for the first model
    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * attributes.vertices.size(), &attributes.vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh_indices.size(), mesh_indices.data(), GL_STATIC_DRAW);

    // Setup for the second model 
    glBindVertexArray(VAOs[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj2_attributes.vertices.size(), &obj2_attributes.vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * obj2_mesh_indices.size(), obj2_mesh_indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    // Perspective matrix
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), height / width, 0.1f, 100.0f);

    // Models
    models.push_back(Model(glm::vec3(0.0f, 0.0f, 0.0f), 0));
    models.push_back(Model(glm::vec3(2.0f, 0.0f, 0.0f), 1));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 viewMatrix = currentCamera->getViewMatrix();

        unsigned int projLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        // Draw the first model (rat)
        models[0].draw(shaderProg, VAOs[0], mesh_indices);

        // Draw the second model (bunny)
        models[1].draw(shaderProg, VAOs[1], obj2_mesh_indices);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteBuffers(2, EBOs);

    glfwTerminate();
    return 0;
}
