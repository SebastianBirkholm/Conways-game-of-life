//includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//resizeing glViewport
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

//input handler
void InputHandler(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main()
{
    //setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //window object creation of the 2 screens, Main and Settings
    GLFWwindow* window = glfwCreateWindow(1600, 600, "Main", NULL, NULL);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }

    //Resize viewport, when the window recizes
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //render loop
    while(!glfwWindowShouldClose(window))
    {
        //Input handeling
        InputHandler(window);

        //Rendering

        //sets clear color and clears screen
        glClearColor(0.5f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Swap buffers and handle events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //terminates window
    glfwTerminate();
    return 0;
}