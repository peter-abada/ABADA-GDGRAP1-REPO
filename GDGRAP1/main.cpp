#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float x = 0.0f, y = 0.0f, z = 0.0f;
float scale_x = 1, scale_y = 1, scale_z = 1;
float axis_x = 0, axis_y = 1, axis_z = 0;
float rotate_x = 0, rotate_y = 0;

static void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_D:
            switch (action) {
                case GLFW_PRESS:
                case GLFW_REPEAT:
                    x += 0.01f;
                    break;
            }
            break;
        case GLFW_KEY_A:
            switch (action) {
                case GLFW_PRESS:
                case GLFW_REPEAT:
                    x -= 0.01f;
                    break;
            }
            break;
        case GLFW_KEY_W:
            switch (action) {
                case GLFW_PRESS:
                case GLFW_REPEAT:
                    y += 0.01f;
                    break;
            }
            break;
        case GLFW_KEY_S:
                switch (action) {
                case GLFW_PRESS:
                case GLFW_REPEAT:
                    y -= 0.01f;
                    break;
            }
            break;
        case GLFW_KEY_LEFT:
            switch (action) {
            case GLFW_PRESS:
            case GLFW_REPEAT:
                rotate_x -= 3.0f;
                axis_y = 1;
                axis_x = 0;
                break;
            }
            break;
        case GLFW_KEY_RIGHT:
            switch (action) {
            case GLFW_PRESS:
            case GLFW_REPEAT:
                rotate_x += 3.0f;
                axis_y = 1;
                axis_x = 0;
                break;
            }
            break;
        case GLFW_KEY_DOWN:
            switch (action) {
            case GLFW_PRESS:
            case GLFW_REPEAT:
                rotate_y += 3.0f;
                axis_y = 0;
                axis_x = 1;
                break;
            }
            break;
        case GLFW_KEY_UP:
            switch (action) {
            case GLFW_PRESS:
            case GLFW_REPEAT:
                rotate_y -= 3.0f;
                axis_y = 0;
                axis_x = 1;
                break;
            }
            break;
        case GLFW_KEY_E:
            switch (action) {
            case GLFW_PRESS:
            case GLFW_REPEAT:
                scale_x += 0.01f;
                scale_y += 0.01f;
                scale_z += 0.01f;
                break;
            }
            break;
        case GLFW_KEY_Q:
            switch (action) {
            case GLFW_PRESS:
            case GLFW_REPEAT:
                scale_x -= 0.01f;
                scale_y -= 0.01f;
                scale_z -= 0.01f;
                break;
            }
            break;
    }
    
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(void)
{
    float height = 600.0f, width = 600.0f;

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Peter Abada", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    int img_width, img_height, colorChannels;

    unsigned char* tex_bytes = stbi_load("3D/ayaya.png", &img_width, &img_height, &colorChannels, 0);

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA,
        img_width,
        img_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tex_bytes);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

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


    std::string path = "3D/djSword.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(&attributes, &shapes, &material, &warning, &error, path.c_str());

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
    }

    std::vector<GLfloat> fullVertexData;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3]);
        fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 1]);
        fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 2]);

        fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2)]);
        fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2 + 1)]);
    }

    GLfloat vertices[]{
        -0.5f, -0.5f, 0,
        0, 0.5f, 0,
        0.5f, -0.5f, 0 
    };

    GLuint indices[] = {
        0,1,2
    };

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    stbi_set_flip_vertically_on_load(true);

    GLuint VAO, VBO, EBO, VBO_UV;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //glGenBuffers(1, &VBO_UV);
    //glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * fullVertexData.size(), fullVertexData.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    GLintptr uvPtr = 3 * sizeof(GLfloat);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)uvPtr);


    /*glBufferData(GL_ARRAY_BUFFER,
                            sizeof(GLfloat) * attributes.vertices.size(),
                            &attributes.vertices[0],
                            GL_STATIC_DRAW);

    glVertexAttribPointer(0,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            3 * sizeof(GLfloat),
                            (void*)0);

    float bytes = (sizeof(GLfloat)) * (sizeof(UV) / sizeof(UV[0]));
    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    glBufferData(
        GL_ARRAY_BUFFER,
        bytes,
        &UV[0],
        GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh_indices.size(), mesh_indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (sizeof(UV) / sizeof(UV[0])), &UV[0], GL_DYNAMIC_DRAW);*/

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat3 identity_matrix3 = glm::mat3(1.0f);
    glm::mat4 identity_matrix4 = glm::mat4(1.0f);

    //Projection matrix
    //glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    //Perspective
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), height / width, 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Camera position / eye
        glm::vec3 cameraPos = glm::vec3(0, 0, 6.0f);
        glm::mat4 cameraPosMatrix = glm::translate(glm::mat4(1.0f), cameraPos * -1.0f);

        glm::vec3 WorldUp = glm::normalize(glm::vec3(0, 1.0f, 0)); // Pointing up
        glm::vec3 Center = glm::vec3(0, 3.0f, 0); // On top of the rabbit
        //
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

        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
        glm::mat4 transformation_matrix = glm::translate(identity_matrix4, glm::vec3(x, y + 0.5, z));
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale_x, scale_y, scale_z));
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotate_y), glm::normalize(glm::vec3(1.0f, 0, 0)));
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotate_x), glm::normalize(glm::vec3(0, 1.0f, 0)));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));
        unsigned int projLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindTexture(GL_TEXTURE_2D, texture);
        GLuint tex0Address = glGetUniformLocation(shaderProg, "tex0");
        glUniform1i(tex0Address, 0);    

        glUseProgram(shaderProg);
        glBindVertexArray(VAO);
        
        /*glDrawElements(GL_TRIANGLES,
                    mesh_indices.size(),
                    GL_UNSIGNED_INT,
                    0); */

        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 5);

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