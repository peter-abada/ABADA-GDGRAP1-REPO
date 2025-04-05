#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "Model.hpp"
#include "OrthoCamera.hpp"
#include "PersCamera.hpp"
#include "DirLight.hpp"
#include "PointLight.hpp"
#include "Shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

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
int selectedModelId = 0; // To check current model (debugging purposes) 

// Mouse callback for camera control using mouse
// Click and hold to move camera around main object
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

// Lights

DirLight dirLight(glm::vec3(4.0f, -5.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

PointLight pointLight(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.1f, 0.2f);


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
            if (selectedModelId) {
                pointLight.Key_Callback(window, key, scancode, action, mods);
            }
            break;
        case GLFW_KEY_1:
            currentCamera = &orthoCamera;
            break;
        case GLFW_KEY_2:
            currentCamera = &persCamera;
            break;
        case GLFW_KEY_LEFT:
        case GLFW_KEY_RIGHT:
            dirLight.Key_Callback(window, key, scancode, action, mods);
            break;
        case GLFW_KEY_UP:
        case GLFW_KEY_DOWN:
            pointLight.Key_Callback(window, key, scancode, action, mods);
            break;
        }
    }
}

/*
    Function to load textures from a file path
    Parameter is the texture file path as a string
    Returns the texture
    Load the texture in main()
*/
GLuint loadTexture(std::string path) {
    int imgWidth, imgHeight, colorChannels;

	unsigned char* tex_bytes = stbi_load(path.c_str(), &imgWidth, &imgHeight, &colorChannels, 0);

	GLuint texture;

	glGenTextures(1, &texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(tex_bytes);

	return texture;
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

    //Table texture
    GLuint texture = loadTexture("3D/tex/rock.jpg");

    //Earth texture
    stbi_set_flip_vertically_on_load(true);
    GLuint texture2 = loadTexture("3D/world5400x2700.jpg");

	int img_width, img_height, colorChannels;
	unsigned char* tex_bytes = stbi_load("3D/tex/rock_normal.jpg", &img_width, &img_height, &colorChannels, 0);

    GLuint norm_tex;
	glGenTextures(1, &norm_tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, norm_tex);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(tex_bytes);

    glEnable(GL_DEPTH_TEST);

    glfwSetKeyCallback(window, Key_Callback);
    glfwSetCursorPosCallback(window, Mouse_Callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Shader for the car models (we don't have a model or texture for this yet)

    Shader carShader("Shaders/sample.vert", "Shaders/sample.frag", "car");
    glLinkProgram(carShader.getProg());

    Shader skyShader("Shaders/skybox.vert", "Shaders/skybox.frag", "sky");
    glLinkProgram(skyShader.getProg());

    std::string path = "3D/racecar.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(&attributes, &shapes, &material, &warning, &error, path.c_str());

    if (!success) {
        std::cerr << "Failed to load model: " << error << std::endl;
        return -1;
    }
	std::cout << "Shapes: " << shapes.size() << std::endl;
    std::vector<GLuint> mesh_indices;


	for (int i = 0; i < shapes.size(); i++) {
        for (int j = 0; j < shapes[i].mesh.indices.size(); j++) {
            mesh_indices.push_back(shapes[i].mesh.indices[j].vertex_index);
        }
	}

	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {
        tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
        tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
        tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

        glm::vec3 v1 = glm::vec3(
            attributes.vertices[(vData1.vertex_index * 3)],
            attributes.vertices[(vData1.vertex_index * 3) + 1],
            attributes.vertices[(vData1.vertex_index * 3) + 2]
        );
        glm::vec3 v2 = glm::vec3(
            attributes.vertices[(vData2.vertex_index * 3)],
            attributes.vertices[(vData2.vertex_index * 3) + 1],
            attributes.vertices[(vData2.vertex_index * 3) + 2]
        );
        glm::vec3 v3 = glm::vec3(
            attributes.vertices[(vData3.vertex_index * 3)],
            attributes.vertices[(vData3.vertex_index * 3) + 1],
            attributes.vertices[(vData3.vertex_index * 3) + 2]
        );

        glm::vec2 uv1 = glm::vec2(
            attributes.texcoords[(vData1.texcoord_index * 2)],
            attributes.texcoords[(vData1.texcoord_index * 2) + 1]
        );
        glm::vec2 uv2 = glm::vec2(
            attributes.texcoords[(vData2.texcoord_index * 2)],
            attributes.texcoords[(vData2.texcoord_index * 2) + 1]
        );
        glm::vec2 uv3 = glm::vec2(
            attributes.texcoords[(vData3.texcoord_index * 2)],
            attributes.texcoords[(vData3.texcoord_index * 2) + 1]
        );

        glm::vec3 deltaPos1 = v2 - v1;
        glm::vec3 deltaPos2 = v3 - v1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }
    
    std::string obj2_path = "3D/world.obj";
    std::vector<tinyobj::shape_t> obj2_shapes;
    std::vector<tinyobj::material_t> obj2_material;

    tinyobj::attrib_t obj2_attributes;

    success = tinyobj::LoadObj(&obj2_attributes, &obj2_shapes, &obj2_material, &warning, &error, obj2_path.c_str());

    if (!success) {
        std::cerr << "Failed to load model: " << error << std::endl;
        return -1;
    }

    std::vector<GLuint> obj2_mesh_indices;
    for (int i = 0; i < obj2_shapes[0].mesh.indices.size(); i++) {
        obj2_mesh_indices.push_back(obj2_shapes[0].mesh.indices[i].vertex_index);
    }


    //Certain objects may not have normal data, this checks if the model does,
    //Otherwise, push an empty index
    
    std::vector<GLfloat> fullVertexData;
    for (int i = 0; i < shapes.size(); i++) {
        for (int j = 0; j < shapes[i].mesh.indices.size(); j++) {
            tinyobj::index_t vData = shapes[i].mesh.indices[j];

            if (vData.vertex_index * 3 + 2 < attributes.vertices.size()) {
                fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3]);
                fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 1]);
                fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 2]);
            }
            else {
                std::cerr << "[OBJ1] Vertex index out of range: vertex_index=" << vData.vertex_index << std::endl;
                fullVertexData.push_back(0.0f);
                fullVertexData.push_back(0.0f);
                fullVertexData.push_back(0.0f);
            }

            if (vData.normal_index * 3 + 2 < attributes.normals.size() && vData.normal_index >= 0) {
                fullVertexData.push_back(attributes.normals[vData.normal_index * 3]);
                fullVertexData.push_back(attributes.normals[vData.normal_index * 3 + 1]);
                fullVertexData.push_back(attributes.normals[vData.normal_index * 3 + 2]);
            }
            else {
                //std::cerr << "[OBJ1] Normal index out of range: normal_index=" << vData.normal_index << std::endl;
                fullVertexData.push_back(0.0f); // Default normal x
                fullVertexData.push_back(1.0f); // Default normal y (pointing up)
                fullVertexData.push_back(0.0f); // Default normal z
            }

            if (vData.texcoord_index * 2 + 1 < attributes.texcoords.size() && vData.texcoord_index >= 0) {
                fullVertexData.push_back(attributes.texcoords[vData.texcoord_index * 2]);
                fullVertexData.push_back(attributes.texcoords[vData.texcoord_index * 2 + 1]);
            }
            else {
                std::cerr << "[OBJ1] Texcoord index out of range: texcoord_index=" << vData.texcoord_index << std::endl;
                fullVertexData.push_back(0.0f);
                fullVertexData.push_back(0.0f);
            }

			// Tangent and bitangent data
			fullVertexData.push_back(tangents[j].x);
			fullVertexData.push_back(tangents[j].y);
			fullVertexData.push_back(tangents[j].z);
			fullVertexData.push_back(bitangents[j].x);
			fullVertexData.push_back(bitangents[j].y);
			fullVertexData.push_back(bitangents[j].z);

        }
    }

    std::vector<GLfloat> obj2_fullVertexData;
    for (int i = 0; i < obj2_shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = obj2_shapes[0].mesh.indices[i];

        if (vData.vertex_index * 3 + 2 < obj2_attributes.vertices.size()) {
            obj2_fullVertexData.push_back(obj2_attributes.vertices[vData.vertex_index * 3]);
            obj2_fullVertexData.push_back(obj2_attributes.vertices[vData.vertex_index * 3 + 1]);
            obj2_fullVertexData.push_back(obj2_attributes.vertices[vData.vertex_index * 3 + 2]);
        }
        else {
            std::cerr << "[OBJ2] Vertex index out of range: vertex_index=" << vData.vertex_index << std::endl;
            obj2_fullVertexData.push_back(0.0f);
            obj2_fullVertexData.push_back(0.0f);
            obj2_fullVertexData.push_back(0.0f);
        }

        if (vData.normal_index * 3 + 2 < obj2_attributes.normals.size() && vData.normal_index >= 0) {
            obj2_fullVertexData.push_back(obj2_attributes.normals[vData.normal_index * 3]);
            obj2_fullVertexData.push_back(obj2_attributes.normals[vData.normal_index * 3 + 1]);
            obj2_fullVertexData.push_back(obj2_attributes.normals[vData.normal_index * 3 + 2]);
        }
        else {
            //std::cerr << "[OBJ2] Normal index out of range: normal_index=" << vData.normal_index << std::endl;
            obj2_fullVertexData.push_back(0.0f); // Default normal x
            obj2_fullVertexData.push_back(1.0f); // Default normal y (pointing up)
            obj2_fullVertexData.push_back(0.0f); // Default normal z
        }

        if (vData.texcoord_index * 2 + 1 < obj2_attributes.texcoords.size() && vData.texcoord_index >= 0) {
            obj2_fullVertexData.push_back(obj2_attributes.texcoords[vData.texcoord_index * 2]);
            obj2_fullVertexData.push_back(obj2_attributes.texcoords[vData.texcoord_index * 2 + 1]);
        }
        else {
            std::cerr << "[OBJ2] Texcoord index out of range: texcoord_index=" << vData.texcoord_index << std::endl;
            obj2_fullVertexData.push_back(0.0f);
            obj2_fullVertexData.push_back(0.0f);
        }

        // Tangent and bitangent data
        fullVertexData.push_back(tangents[i].x);
        fullVertexData.push_back(tangents[i].y);
        fullVertexData.push_back(tangents[i].z);
        fullVertexData.push_back(bitangents[i].x);
        fullVertexData.push_back(bitangents[i].y);
        fullVertexData.push_back(bitangents[i].z);

    }

    float skyboxVertices[]{
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
    };

    unsigned int skyboxIndices[]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };

    //Skybox
    unsigned int skyVAO, skyVBO, skyEBO;

    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glGenBuffers(1, &skyEBO);

    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    std::string facesSky[]{
        "Skybox/rainbow_rt.png",
        "Skybox/rainbow_lf.png",
        "Skybox/rainbow_up.png",
        "Skybox/rainbow_dn.png",
        "Skybox/rainbow_ft.png",
        "Skybox/rainbow_bk.png",
    };

    unsigned int skyTex;

    glGenTextures(1, &skyTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyTex);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {
        int w, h, skyChannel;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(facesSky[i].c_str(), &w, &h, &skyChannel, 0);

        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }

    stbi_set_flip_vertically_on_load(true);

    //Objects
    GLuint VAOs[2], VBOs[2];

    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);

    // Setup for the first model
    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * fullVertexData.size(), fullVertexData.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    GLintptr normPtr1 = 3 * sizeof(GLfloat);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)normPtr1);
    glEnableVertexAttribArray(1);

    GLintptr uvPtr1 = 6 * sizeof(GLfloat);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)uvPtr1);
    glEnableVertexAttribArray(2);

	GLintptr tangentPtr = 8 * sizeof(float);
	GLintptr bitangentPtr = 11 * sizeof(float);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)tangentPtr);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)bitangentPtr);

    // Setup for the second model 
    glBindVertexArray(VAOs[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj2_fullVertexData.size(), obj2_fullVertexData.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    GLintptr normPtr2 = 3 * sizeof(GLfloat);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)normPtr2);
    glEnableVertexAttribArray(1);

    GLintptr uvPtr2 = 6 * sizeof(GLfloat);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)uvPtr2);
    glEnableVertexAttribArray(2);

	glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

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

        //Render sky
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyShader.getProg());

        glm::mat4 skyView = glm::mat4(1.0f);
        skyView = glm::mat4(glm::mat3(viewMatrix));

        unsigned int skyViewLoc = glGetUniformLocation(skyShader.getProg(), "view");
        glUniformMatrix4fv(skyViewLoc, 1, GL_FALSE, glm::value_ptr(skyView));

        unsigned int skyProjLoc = glGetUniformLocation(skyShader.getProg(), "projection");
        glUniformMatrix4fv(skyProjLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(skyVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        //Render objects
        glUseProgram(carShader.getProg());

        unsigned int projLoc = glGetUniformLocation(carShader.getProg(), "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        unsigned int viewLoc = glGetUniformLocation(carShader.getProg(), "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		glActiveTexture(GL_TEXTURE0);
        GLuint tex0Address = glGetUniformLocation(carShader.getProg(), "tex0");
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(tex0Address, 0);

		glActiveTexture(GL_TEXTURE1);
		GLuint normTexAddress = glGetUniformLocation(carShader.getProg(), "norm_tex");
		glBindTexture(GL_TEXTURE_2D, norm_tex);
		glUniform1i(normTexAddress, 1);

        GLuint lightDirAddress = glGetUniformLocation(carShader.getProg(), "lightDir");
        glUniform3fv(lightDirAddress, 1, glm::value_ptr(dirLight.getLightDir()));

        GLuint lightColorAddress = glGetUniformLocation(carShader.getProg(), "lightColor");
        glUniform3fv(lightColorAddress, 1, glm::value_ptr(dirLight.getLightColor()));

        GLuint pointLightPosAddress = glGetUniformLocation(carShader.getProg(), "pointLightPos");
        glUniform3fv(pointLightPosAddress, 1, glm::value_ptr(pointLight.getPosition()));

        GLuint pointLightColorAddress = glGetUniformLocation(carShader.getProg(), "pointLightColor");
        glUniform3fv(pointLightColorAddress, 1, glm::value_ptr(pointLight.getLightColor()));

        GLuint pointLightConstantAddress = glGetUniformLocation(carShader.getProg(), "pointLightConstant");
        glUniform1f(pointLightConstantAddress, pointLight.getConstant());

        GLuint pointLightLinearAddress = glGetUniformLocation(carShader.getProg(), "pointLightLinear");
        glUniform1f(pointLightLinearAddress, pointLight.getLinear());

        GLuint pointLightQuadraticAddress = glGetUniformLocation(carShader.getProg(), "pointLightQuadratic");
        glUniform1f(pointLightQuadraticAddress, pointLight.getQuadratic());

        GLuint dirLightIntensityAddress = glGetUniformLocation(carShader.getProg(), "dirLightIntensity");
        glUniform1f(dirLightIntensityAddress, dirLight.getIntensity());

        GLuint pointLightIntensityAddress = glGetUniformLocation(carShader.getProg(), "pointLightIntensity");
        glUniform1f(pointLightIntensityAddress, pointLight.getIntensity());

        // Draw the first model (table)
        models[0].draw(carShader.getProg(), VAOs[0], mesh_indices, fullVertexData);

        // Draw the second model (earth)
        glBindTexture(GL_TEXTURE_2D, texture2);
        models[1].draw(carShader.getProg(), VAOs[1], obj2_mesh_indices, obj2_fullVertexData);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);

    glfwTerminate();
    return 0;
}
