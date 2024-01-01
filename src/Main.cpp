#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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
    GLFWwindow* window2 = glfwCreateWindow(600, 600, "Settings", NULL, NULL);;

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }

    //Resize viewport, when the window recizes
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetFramebufferSizeCallback(window2, framebuffer_size_callback);

    //render loop
    while(!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwSwapBuffers(window2);
        glfwPollEvents();
    }

    //terminates window
    glfwTerminate();
    return 0;
}