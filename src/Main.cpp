//Opengl libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//math stuff
#include <glm/glm/glm.hpp>

//writing to the console
#include <iostream>

//shader library
#include <shader.hpp>

//function declaration
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;
const unsigned int amount = 10000;
static GLfloat* testing = new GLfloat[amount*amount * 3];

const char *fragmentShaderSource= "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n"
    "}\0";

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec3 aOffset;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4((aPos + aOffset.xy)*aOffset.z, 0.0, 1.0);\n"
    "}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "🙃", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glEnable(GL_DEPTH_TEST);

    float Vertices[] = {
        -0.5f,  0.5f, 0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, 0.5f, -0.5f, 0.5f,  0.5f
    };
    //generate and bind the vertex array
    unsigned int VAO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //generate vertices array
    unsigned int verticesBuffer;

    //Write to the vertex array
    glGenBuffers(1, &verticesBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    //buffer base locations normilised to 0,0 to the vertex shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // bind the shader program
    glUseProgram(shaderProgram);

    //update the uniform color
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");     
    glUniform4f(vertexColorLocation, 0.0f, 255.0f, 0.0f, 1.0f);

    //initialse and write the offset buffer
    unsigned int offsetBuffer;

    glGenBuffers(1, &offsetBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, offsetBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * pow(amount,2), NULL, GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //generate bounch of offsets and size
        int index = 0;
        float offset = 0.5f;
        for (int y = 0; y < amount; y += 1)
        {
            for (int x = 0; x < amount; x += 1)
            {
                testing[3*index] = (float)x + offset;
                testing[3*index+1] = (float)y + offset;
                testing[3*index+2] = 0.1f;
                index++;
            }
        }

        //buffer the data
        glBindBuffer(GL_ARRAY_BUFFER, offsetBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * pow(amount,2), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * pow(amount,2), testing);

        //Write the generated data in the offset buffer to the vertex shader
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        //tell the vertex shader that its instanced data
        glVertexAttribDivisor(1, 1);

        //draw the triangles
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, pow(amount,2));

        //Swap the buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

//Function definitions

//Activates when someone resizes the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}