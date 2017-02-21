#ifndef GFX_UTILS
#define GFX_UTILS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "camera.h"
#include "../Utilities/utils.h"
#include "../Models/Penguin.h"
#include "../Models/Coin.h"
#include "../Models/Tree.h"
#include "../Models/GameScene.h"
#include "shaders.h"
#include <irrKlang/irrKlang.h>

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
    void static key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
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
        // Setup some OpenGL options
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        return 0;
    }
    penguinMovement do_movement(GLfloat deltaTime, bool& movingForward, bool& movingBackward, bool& movingRight, bool& movingLeft)
    {
        if(keys[GLFW_KEY_W])
            camera->ProcessKeyboard(FORWARD, deltaTime);
        if(keys[GLFW_KEY_S])
            camera->ProcessKeyboard(BACKWARD, deltaTime);
        if(keys[GLFW_KEY_A])
            camera->ProcessKeyboard(LEFT, deltaTime);
        if(keys[GLFW_KEY_D])
            camera->ProcessKeyboard(RIGHT, deltaTime);
        if(keys[GLFW_KEY_UP])
            movingForward = true;
        if(keys[GLFW_KEY_DOWN])
            movingBackward = true;
        if(keys[GLFW_KEY_RIGHT])
            movingRight = true;
        if(keys[GLFW_KEY_LEFT])
            movingLeft = true;
        return NO_MOVEMENT;
    }
    
    void score(vector<Lane> &lanesArray, long &score, Penguin* pen, irrklang::ISoundEngine* engine) {
        bool isCollided = false;
        float penRightPos = pen->getPenX() + (1.492 * 0.13);
        float penLeftPos = pen->getPenX() - (1.492 * 0.13);
        float coinRightPos, coinLeftPos;
        //adding +1 to score if not visited Lane
        if (lanesArray[pen->getCurrentLane()].type) {
            if (!(lanesArray[pen->getCurrentLane()].isVisited)) {
                score++;
                lanesArray[pen->getCurrentLane()].isVisited = true;
            }
        }
        if (lanesArray[pen->getCurrentLane()].hasCoin && !lanesArray[pen->getCurrentLane()].isCoinConsumed) {
            coinRightPos = lanesArray[pen->getCurrentLane()].coinXPosition + 0.4;
            coinLeftPos = lanesArray[pen->getCurrentLane()].coinXPosition - 0.4;
            
            if ((penLeftPos < coinRightPos) && (penRightPos >= coinLeftPos)) {
                isCollided = true;
            }
            if ((penRightPos >= coinLeftPos) && (penLeftPos < coinRightPos)) {
                isCollided = true;
            }
            if(isCollided) {
                lanesArray[pen->getCurrentLane()].isCoinConsumed = true;
                engine->play2D("/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Sounds/346404__robinhood76__06698-gem-collect-ding.wav");
            }
        }
        //adding +100 to score if collided with a coin
        if (isCollided) {
            score += 10;
        }
    }
    void bindCube(GLuint & VBO, GLuint & VAO, bool isSafeLane = false){
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Set vectices
        if(!isSafeLane){
            GLfloat vertices[] = {
                // Front face
                -25, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
                25, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, // bottom-right
                25,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
                25,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
                -25,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f, // top-left
                -25, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
                
                // Top face
                -25,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top-left
                25,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
                25,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // top-right
                25,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
                -25,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top-left
                -25,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f  // bottom-left
            };
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
        }else{
            GLfloat vertices[] = {
                // Front face
                -25, -0.2f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
                25, -0.2f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, // bottom-right
                25,  0.8f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
                25,  0.8f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
                -25,  0.8f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f, // top-left
                -25, -0.2f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
                // Top face
                -25,  0.8f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top-left
                25,  0.8f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
                25,  0.8f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // top-right
                25,  0.8f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
                -25,  0.8f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top-left
                -25,  0.8f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f  // bottom-left
            };
            
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
        }
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
        
    }
    void RenderQuad(Shader &debugDepthQuad, GLuint &quadVAO, GLuint &quadVBO,GLfloat near_plane, GLfloat far_plane, GLuint &depthMap)
    {
        // 3. DEBUG: visualize depth map by rendering it to plane
        debugDepthQuad.Use();
        glUniform1f(glGetUniformLocation(debugDepthQuad.Program, "near_plane"), near_plane);
        glUniform1f(glGetUniformLocation(debugDepthQuad.Program, "far_plane"), far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        //               RenderQuad(); // uncomment this line to see depth map
        if (quadVAO == 0)
        {
            GLfloat quadVertices[] = {
                // Positions        // Texture Coords
                -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
                1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
                1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
            };
            // Setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
    void DepthMapInitialization(GLuint &depthMapFBO, GLuint &depthMap,GLuint SHADOW_WIDTH = 4000, GLuint SHADOW_HEIGHT = 4000){
        glGenFramebuffers(1, &depthMapFBO);
        // - Create depth texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void renderEverything(Shader &simpleDepthShader, glm::mat4 &lightProjection,glm::mat4  &lightView, glm::mat4 &lightSpaceMatrix,GLfloat &near_plane ,GLfloat &far_plane, GameScene &gameScene, Penguin &penguin,bool &movingForward,bool &movingBackward, bool &movingRight,bool &movingLeft, vector<Lane> &lanesArray,GLuint &frameCount,GLfloat &deltaTime, GLuint &depthMapFBO, GLuint &depthMap, GraphicsUtilities &graphicsUtilities, Car &car, Car &truck, Coin &coin,Tree &tree, GLuint &VAO, GLuint  &VAOSafeLane, glm::vec3 lightPos, float looktAtX, float looktAtY, float looktAtZ, Camera& camera){
        
        lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(looktAtX, looktAtY, looktAtZ), glm::vec3(0.0, 1.0, 0.0));
        //Light Space Matrix has the view and the projection
        lightSpaceMatrix = lightProjection * lightView;
        // Now render scene from light's point of view
        simpleDepthShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glViewport(0, 0, Constants::SHADOW_WIDTH, Constants::SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        //Render The Penguin
        graphicsUtilities.do_movement(deltaTime, movingForward, movingBackward, movingRight, movingLeft);
        penguin.move(movingForward, movingBackward, movingRight, movingLeft,camera, deltaTime, lanesArray);
        penguin.render(simpleDepthShader, frameCount, (movingForward || movingRight || movingLeft || movingBackward), deltaTime, lanesArray);
        //Render Gamescene(Lanes, Coins, Trees, Cars)
        gameScene.render(simpleDepthShader, lanesArray, VAO, VAOSafeLane, car, truck, coin, tree);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

// Static
bool GraphicsUtilities::keys[1024];
GLfloat GraphicsUtilities::lastX;
GLfloat GraphicsUtilities::lastY;
GLboolean GraphicsUtilities::firstMouse;
Camera* GraphicsUtilities::camera;
#endif
