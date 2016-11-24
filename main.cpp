#include <iostream>
#include <GL/glew.h>
#include <cmath>
#include <GLFW/glfw3.h>
#include "shaders.h"
#include "camera.h"
#include <SOIL.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
float x = 0.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool keys[1024];
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 4.0f, 5.0f));
GLfloat lastX = 400, lastY = 300;
GLboolean firstMouse = true;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

GLint lanesCount = 20;

void do_movement()
{
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode)
{
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

GLint laneHeight = 0;
const GLint gameWidth = 400;
const GLint gameHeight = gameWidth* 1.78;
int main(int argc, const char * argv[]) {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // For full screen
    //    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    //    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    //    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    //    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    //    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    //
    //    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "CrossyRoad", glfwGetPrimaryMonitor(), NULL);
    //    laneHeight = mode->height / 4;
    
    // Set some constants
    laneHeight = gameHeight / 4;
    
    // Create and configure window
    GLFWwindow* window = glfwCreateWindow(gameWidth, gameHeight, "CrossyRoad", NULL, NULL);
    
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glewExperimental = GL_TRUE;
    //glViewport(0, 0, mode->width, mode->height);
    
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    // Load shaders (normal shader and light source shader)
    Shader ourShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/shader.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/shader.frag");
    
    // Set vectices
    float initialY =-1 + 6.0/lanesCount;
    glEnable(GL_DEPTH_TEST);
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // Bind vertex buffer object and vertex array object and set buffer data from vertices object
    // Set the attributes in shaders
    
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    
    // Get yOffset uniform location (to draw 4 lanes)
    GLint yOffset = glGetUniformLocation(ourShader.Program, "yOffset");
    GLint safeLand = glGetUniformLocation(ourShader.Program, "safeLand");
    
    // ToDo: Algorithm to generate these 2 arrays with the following conditions:
    // • lanes[]: random sequence of 0's and 1's while maintaining the following:
    //              - No more than 2 safe lands next to each other
    //              - No more than 5 normal lanes next to each other
    //              - Start with a safe lane
    // •lanesColorsAndSeparators[]: flags for coloring the lanes based on the following:
    //                              - 0 by default
    //                              - 1 if the current lane is safe lane and the next is safe lane too
    //                              - 2 if current is normal lane and next is normal lane too
    
    int lanes[] = {0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0 ,1 ,1,  1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0}; // 0 safe lane, 1 normal lane
    
    //    int lanesColorsAndSeparators[] =
    //                  {0, 2, 0, 0, 1, 2, 2, 2, 0, 0, 1, 2, 2, 0, 0, 1, 2, 2, 2, 2, 2, 0, 1, 0, 2, 2, 2, 2, 2,
    //        1, 0, 2, 2, 2, 2, 2, 0, 1, 0, 2, 2, 0, 1, 0, 2, 2, 2, 2, 2, 0, 1, 1};
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents(); // Check for keyboard or mouse events
        // Calculate frame time for smooth/quick movement on all devices
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Move the camera around if any key clicked
        do_movement();
        
        // Clear screen with certain background color
        glClearColor(0.62f, 0.62f, 0.62f, 1.0f);
        // Clear depth and color buffers with each iteration so the new values can be applied without blocking
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Indicate to gl that we are going to use the original shaders for now
        ourShader.Use();
        
        // Set model, view, projection matrices
        
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float) gameHeight/(float)gameWidth, 0.1f, 100.0f);
        
        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        GLint projectionLoc = glGetUniformLocation(ourShader.Program, "projection");
        
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(VAO);
        
        
        GLint isSafeLaneLoc = glGetUniformLocation(ourShader.Program, "isSafeLane");
        GLint isSafeLaneAfterSafeLane = glGetUniformLocation(ourShader.Program, "isSafeLaneAfterSafeLane");
        GLint isNormalLaneBeforeNormalLane = glGetUniformLocation(ourShader.Program, "isNormalLaneBeforeNormalLane");
        
        //Draw cubes
        float zpos = 0;
        for(GLuint i = 0; i < 30; i++)
        {
            glm::mat4 model;
            zpos -= 1;
            model = glm::translate(model, glm::vec3( 0.0f, 0.0f, zpos));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            
            if(lanes[i] == 0){
                glUniform1f(isSafeLaneLoc, 1.0f);
            }
            if(i > 0 && lanes[i] == 0 && lanes[i-1] == 0){
                glUniform1f(isSafeLaneAfterSafeLane, 1.0f);
            }
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
            // Draw lane separator
            if(i < 29 && lanes[i] == 1 && lanes[i+1] == 1){
                zpos -= 1.1;
                glm::mat4 model;
                model = glm::translate(model, glm::vec3( 0.0f, 0.0f, zpos));
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.08f));
                glUniform1f(isNormalLaneBeforeNormalLane, 1.0f);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                zpos -= 0.1;
            }
            
            
            // Reset uniforms
            glUniform1f(isSafeLaneLoc, 0.0f);
            glUniform1f(isSafeLaneAfterSafeLane, 0.0f);
            glUniform1f(isNormalLaneBeforeNormalLane, 0.0f);

        }
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
