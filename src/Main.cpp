//Opengl libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//math stuff
#include <glm/glm/glm.hpp>
#include <cmath>

//writing to the console
#include <iostream>

//shader library
#include <shader.hpp>

//settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

//creates global variables
const unsigned int amount = 3 + 2;
static GLfloat* testing = new GLfloat[(amount-2)*(amount-2) * 3];
static float size = 0.1f;

//function declaration
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool CheckNeighbors(bool (*temp)[amount][amount], int x, int y);

//creates cell array
bool cells[amount][amount] = {true};
bool tempCells[amount][amount] = {false};

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
    //variable declaration
    float offset = 0.5f;
    int neighborCounter = 0;

    //initialise GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //creates the window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "🙃", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
    }

    //makes the current window the focus
    glfwMakeContextCurrent(window);

    //set the resize screen callback function
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //loades Glad
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

    for (int i = 1; i < amount-1; i++)
    {
        for (int j = 1; j < amount-1; j++)
        {
            if(j%2==0) cells[i][j] = true;
            else cells[i][j] = false;
        }
    }
    

    while (!glfwWindowShouldClose(window))
    {
        //variable declaration
        int index = 0;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //updates the array and draws the live cells to the screen
        for (int y = 1; y < amount-1; y++)
        {
            for (int x = 1; x < amount-1; x++)
            {
                //checks if a cell is alive
                if (cells[y][x])
                {
                    //draws it to the graphics buffer
                testing[3*index] = (float)x + offset;
                testing[3*index+1] = (float)y + offset;
                testing[3*index+2] = size;
                index++;
                }

                //cell rules execution
                //*pTempCell[x][y] = CheckNeighbors(pCell,x,y);
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
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, index);

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

//check neighbors
bool CheckNeighbors(int x, int y)
{
    //amount of alive neighbors
    int counter = 0;

    //checks around the cell
    for (int i = 1; i < 9; i++)
    {
        if(cells[x-(int)round(cos((6.28/8)*i))][y-(int)round(sin((6.28/8)*i))]) counter++;
    }

    return false;
}