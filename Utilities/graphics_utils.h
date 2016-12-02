#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "camera.h"

using namespace std;

enum penguinMovement{
    FORWARD_MOVEMENT,
    BACKWARD_MOVEMENT,
    LEFT_MOVEMENT,
    RIGHT_MOVEMENT,
    NO_MOVEMENT
};
class GraphicsUtilities {
private:
    void static key_callback(GLFWwindow* window, int key, int scancode, int action,
                             int mode)
    {
        if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
    void static mouse_callback(GLFWwindow* window, double xpos, double ypos){
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
        
        camera->ProcessMouseMovement(xoffset, yoffset);
    }
    
    void static scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        camera->ProcessMouseScroll(yoffset);
    }
    
    
public:
    
    static bool keys[1024];
    static GLfloat lastX;
    static GLfloat lastY;
    static GLboolean firstMouse;
    static Camera* camera;
    
    GraphicsUtilities(Camera* c){
        camera = c;
        GraphicsUtilities::lastX = 400;
        lastY = 300;
        firstMouse = true;
    }
    int initializeGameWindow(int width, int height, int glfwMajor, int glfwMinor, GLFWwindow*& window){
        // Initialize GLFW and window
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwMinor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        
        
        
        window = glfwCreateWindow(width, height, "CrossyRoad", NULL, NULL);
        
        // Create and configure window
        if (window == nullptr)
        {
            cout << "Failed to create GLFW window" << endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        
        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        
        // GLEW
        glewExperimental = GL_TRUE;
        
        if (glewInit() != GLEW_OK)
        {
            cout << "Failed to initialize GLEW" << endl;
            return -1;
        }
        glEnable(GL_DEPTH_TEST);
        
        return 0;
    }
    penguinMovement do_movement(GLfloat deltaTime)
    {
        if(keys[GLFW_KEY_Q])
            camera->ProcessKeyboard(FRONT, deltaTime);
        if(keys[GLFW_KEY_E])
            camera->ProcessKeyboard(BACK, deltaTime);
        if(keys[GLFW_KEY_W])
            camera->ProcessKeyboard(FORWARD, deltaTime);
        if(keys[GLFW_KEY_S])
            camera->ProcessKeyboard(BACKWARD, deltaTime);
        if(keys[GLFW_KEY_A])
            camera->ProcessKeyboard(LEFT, deltaTime);
        if(keys[GLFW_KEY_D])
            camera->ProcessKeyboard(RIGHT, deltaTime);
        if(keys[GLFW_KEY_UP])
            return FORWARD_MOVEMENT;
        if(keys[GLFW_KEY_DOWN])
            return BACKWARD_MOVEMENT;
        if(keys[GLFW_KEY_RIGHT])
            return RIGHT_MOVEMENT;
        if(keys[GLFW_KEY_LEFT])
            return LEFT_MOVEMENT;
        return NO_MOVEMENT;
    }
    void bindCube(GLuint & VBO, GLuint & VAO){
        // Set vectices
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
        
    }
};

// Static
bool GraphicsUtilities::keys[1024];
GLfloat GraphicsUtilities::lastX;
GLfloat GraphicsUtilities::lastY;
GLboolean GraphicsUtilities::firstMouse;
Camera* GraphicsUtilities::camera;
