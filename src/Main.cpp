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
const unsigned int amount = 9 + 2;
static GLfloat* testing = new GLfloat[(amount-2)*(amount-2) * 3];
float size = 0.1f;


//function declaration
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool CheckNeighbors(int x, int y);

//creates cell array
bool cells[amount][amount];
bool tempCells[amount][amount];

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
    float currentTime = 0;
    float lastTime = 0;
    int index = 0;
    bool timeActive = false;
    bool moveForwardManual = false;

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

    //For testing
    for (int i = 1; i < amount-1; i++)
    {
        for (int j = 1; j < amount-1; j++)
        {
            cells[i][j] = true;
        }
    }
    

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Tick controller
        currentTime += glfwGetTime() - lastTime; 
        lastTime = glfwGetTime();

        //Waits till 5 seconds have passed
        std::cout << currentTime << std::endl;
        if ((currentTime > 5.0f && timeActive) || moveForwardManual)
        {
            //resets moveForwardManual
            moveForwardManual = false;

            //resets current time
            currentTime = 0;

            //resets index
            index = 0;

            //updates the array and draws the live cells to the screen
            for (int y = 1; y < amount-1; y++)
            {
                for (int x = 1; x < amount-1; x++)
                {
                    //updates the temp array based on Conways rules
                    tempCells[y][x] = CheckNeighbors(x,y);

                    //checks if a cell is alive
                    if (!cells[y][x]) continue;
                
                    //draws it to the graphics buffer
                    testing[3*index] = (float)(x-1) + offset;
                    testing[3*index+1] = (float)(y-1) + offset;
                    testing[3*index+2] = size;
                    
                    //counts index up
                    index++;
                }
            }

            //Updates the cell array
            std::copy(&tempCells[0][0], &tempCells[0][0]+amount*amount, &cells[0][0]);
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
        if(cells[y-(int)round(cos((6.28/8)*i))][x-(int)round(sin((6.28/8)*i))]) counter++;
    }

    //Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
    if (!cells[y][x] && counter == 3)
    {
        return true;
    }
    
    //Any live cell with fewer than two live neighbours dies, as if by underpopulation.
    //Any live cell with more than three live neighbours dies, as if by overpopulation.
    if (cells[y][x] && (counter<2 || counter > 3))
    {
        return false;
    }

    //Any live cell with two or three live neighbours lives on to the next generation. 
    if (cells[y][x] && (counter == 2 || counter == 3))
    {
        return true;
    }

    //default for exceptions
    return false;
}