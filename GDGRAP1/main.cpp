#include <GLFW/glfw3.h>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Peter Abada", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //Top triangle
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.2f, 1.0f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.2f, 1.0f);
        glEnd();
        //Bottom triangle
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.2f, 0.0f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.2f, 0.0f);
        glEnd();
        //Left triangle
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.4f, 0.3f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(-0.4f, 0.7f);
        glEnd();
        //Right triangle
        glBegin(GL_TRIANGLES);
        glVertex2f(0.4f, 0.3f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.4f, 0.7f);
        glEnd();
        //Bottom-left triangle
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.4f, 0.3f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(-0.2f, 0.0f);
        glEnd();
        //Bottom-right triangle
        glBegin(GL_TRIANGLES);
        glVertex2f(0.4f, 0.3f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.2f, 0.0f);
        glEnd();
        //Top-left triangle
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.4f, 0.7f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(-0.2f, 1.0f);
        glEnd();
        //Top-right triangle
        glBegin(GL_TRIANGLES);
        glVertex2f(0.4f, 0.7f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.2f, 1.0f);
        glEnd();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}