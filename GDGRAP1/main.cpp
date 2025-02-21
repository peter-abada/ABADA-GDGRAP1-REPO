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

TODO:

1. CREATE A MODEL CLASS                  -              SOMEWHAT FINISHED, FOR NOW IT JUST PUTS THEM RANDOMLY ON THE SCREEN:
    ATTRIBUTES:
        Position (x, y, z)
        Rotation (x, y, z)
        Scale (x, y, z)

    METHODS:
        draw() - draws the object

2. IMPLEMENT A CAMERA SYSTEM:
    - Either mouse or arrow controls camera
    - WASD to control movement
    - SPACE spawns a 3D model in front of the player with 3 second cooldown
        - Previously spawned models should persist

3. OTHER SPECS:
    - Use your own OBJ file (we can download one)
    - The model class should store the position of each model object
    - Use only one vertex and fragment shader
    - Use perspective projection (See slides "05GDGRAP1-Projections")
        - Related code can be found commented in main()
    - Models should spawn directly in front of the camera
    - One model should be spawned on open
    - Document code
    - Credit 3D model source


4. MAKE DRAW MODEL WORK WITH CAMERA PROBABLY IN LIKE THE KEY_CALLBACK FUNCTIONS

*/



class Model {
private:

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

public:

    Model(glm::vec3 pos) : position(pos), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f) {}

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

std::vector<Model> models;

float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 5.0f;

static void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_SPACE:
            {
            float randX = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
            float randY = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
            models.push_back(Model(glm::vec3(randX, randY, 0.0f)));
            break;
            }
        case GLFW_KEY_W:
            cameraX += 0.1f;
            break;
        case GLFW_KEY_S:
            cameraX -= 0.1f;
            break;
        case GLFW_KEY_A:
            cameraY -= 0.1f;
            break;
        case GLFW_KEY_D:
            cameraY += 0.1f;
            break;
        }
    }
}

int main(void) {
    srand(static_cast<unsigned int>(time(0)));

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

    std::string path = "3D/bunny.obj";
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

    //Projection matrix
    //glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    //Perspective
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
        glm::vec3 Center = glm::vec3(0, 3.0f, 0); // On top of the rabbit
        
        glm::vec3 Forward = Center - cameraPos;
        Forward = glm::normalize(Forward);

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
