#include <cmath>
#include <fstream>
#include <map>
#include <SOIL/SOIL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Utilities/graphics_utils.h"
#include "Utilities/fonts.h"
#include "Model/PenModel.h"
#include "Models/GameScene.h"
#include "Models/Penguin.h"
#include <irrKlang/irrKlang.h>
using namespace std;

GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 4.0f, 5.0f));

// Configure depth map FBO (UHD 3840×2160) resolution for better shadow Appearance
const GLuint SHADOW_WIDTH = 4400, SHADOW_HEIGHT = 4000;
const GLint gameWidth = 750;
const GLint gameHeight = 800;
long score = 0;
bool gameOver = false;
bool exitGame = false;
bool gameOverSoundPlayed= false;
GLFWwindow* window;

float lx = -22, ly = 40, lz = -30,ax = 0,ay = 0,az = -30;
glm::vec3 lightPos(lx, ly, lz);

collisionStatus collisionState;

//For Rendering the DepthMap (Debuging and testing)
GLuint quadVAO = 0;
GLuint quadVBO;


void RenderEverything(Shader &simpleDepthShader, glm::mat4 &lightProjection,glm::mat4  &lightView, glm::mat4 &lightSpaceMatrix,GLfloat &near_plane ,GLfloat &far_plane, GameScene &gameScene, Penguin &penguin,bool &movingForward,bool &movingBackward, bool &movingRight,bool &movingLeft, vector<lane> &lanesArray,GLuint &frameCount,GLfloat &deltaTime, GLuint &depthMapFBO, GLuint &depthMap, float &newStart, GraphicsUtilities &graphicsUtilities, Car &car, Car &truck, Coin &coin,Tree &tree, GLuint &VAO, GLuint  &VAOSafeLane);

int main(int argc, const char * argv[]) {
    
    // initialize game
    GraphicsUtilities graphicsUtilities(&camera);
    if(0 != graphicsUtilities.initializeGameWindow(gameWidth, gameHeight, 3, 3, window)){
        return -1;
    }
    // Load SHADERS
    Shader sceneShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/shader.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/shader.frag");
    Shader materialShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/objshader.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/objshader.frag");
    Shader textureShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/shadow_mapping.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/shadow_mapping.frag");
    Shader fontShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/font.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/font.frag");
    Shader simpleDepthShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/shadow_mapping_depth.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/shadow_mapping_depth.frag");
    
    // start the sound engine with default parameters
    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    
    if (!engine)
    {
        printf("Could not startup engine\n");
        return 0; // error starting up the engine
    }
    
    // Options
    GLboolean shadows = true;
    
    
    // Load gamescene
    GameScene gameScene(sceneShader);
    // Load MODELS
    Penguin penguin(materialShader, "/Users/ibrahimradwan/Desktop/penguin.dae", camera);
    penguin.setPenPosition(0, penguin.getPenguinConstantY(), -3.3f); // Set initial posisiton
    Car car(textureShader, "/Users/ibrahimradwan/Desktop/Small_car_obj/Small car.obj");
    Car truck(textureShader, "/Users/ibrahimradwan/Desktop/cubus_deutz_rund/tlf16_rund.obj");
    Coin coin(textureShader ,"/Users/ibrahimradwan/Desktop/coin/Gems/diamond_orange.obj");
    Tree tree(textureShader, "/Users/ibrahimradwan/Desktop/Tree/tree_4.obj");
    
    
    float newStart = -1.3;
    // Generate lanes
    vector<lane> lanesArray;
    Utilities::generateLanesAlgorithm(lanesArray);
    
    // Some variables for game logic go here::
    GLuint frameCount = 0;
    
    // Loading fonts
    Fonts fonts(fontShader, gameWidth, gameHeight, "/Users/ibrahimradwan/Desktop/zorque.ttf");
    
    // Load the lane-cube into the fragment
    GLuint VBO, VAO;
    graphicsUtilities.bindCube(VBO, VAO);
    GLuint VBOSafeLane, VAOSafeLane;
    graphicsUtilities.bindCube(VBOSafeLane, VAOSafeLane, true);
    
    //DepthMap initialization
    GLuint depthMapFBO;
    //depth texture
    GLuint depthMap;
    //DeapthMap Buffer Bindings
    graphicsUtilities.DepthMapInitialization(depthMapFBO, depthMap, SHADOW_WIDTH, SHADOW_HEIGHT);
    
    //depthMapInitializtion(depthMap, depthMapFBO,SHADOW_WIDTH,SHADOW_HEIGHT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    //play Background Music
    
    engine->play2D("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Sounds/CarHornBase.wav", true);
    engine->play2D("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Sounds/180156__klankbeeld__traffic-horns-city-nervous-busy.wav", true);
    engine->play2D("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Sounds/MagicMatch_GameTheme.ogg", true);
    
    while(!glfwWindowShouldClose(window) && !exitGame)
    {
        glfwPollEvents(); // Check for keyboard or mouse events
        
        // Calculate frame time for smooth/quick movement on all devices
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        
        // 1. Render depth of scene to texture (from light's perspective)
        //Get light projection view matrix
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        GLfloat near_plane = -1.0f, far_plane = 60.0f;
        lightPos = glm::vec3(lx, ly, lz);
        
        //Penguin Movement Attributes
        bool movingForward = false, movingBackward = false, movingRight = false, movingLeft = false;
        
        //Call Render Everything functuion
        RenderEverything(simpleDepthShader, lightProjection, lightView, lightSpaceMatrix, near_plane, far_plane, gameScene, penguin, movingForward, movingBackward, movingRight, movingLeft, lanesArray, frameCount, deltaTime, depthMapFBO, depthMap, newStart, graphicsUtilities, car, truck, coin, tree, VAO, VAOSafeLane);
        
        //Update the Light Pos And the Light Look at
        lz = penguin.getPenZ() + 8.3;
        az = penguin.getPenZ() + 36.3;
        
        // Clear screen with certain background color
        glViewport(0, 0, gameWidth*2, gameHeight*2);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        // Clear depth and color buffers with each iteration so the new values can be applied without blocking
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        // Sart drawing
        
        // Penguin drawing
        graphicsUtilities.do_movement(deltaTime, movingForward, movingBackward, movingRight, movingLeft);
        penguin.move(movingForward, movingBackward, movingRight, movingLeft,camera, deltaTime, newStart, lanesArray);
        
        // penguin drawing
        penguin.draw(camera.GetViewMatrix(), glm::radians(camera.Zoom), (float)gameHeight/gameWidth, 0.1f, 1000.0f, frameCount, (movingForward || movingRight || movingLeft || movingBackward), deltaTime, lanesArray);
        
        // Draw the scene (lanes + cars + Diamonds + trees)
        gameScene.draw(lightSpaceMatrix, depthMap, camera, lightPos, shadows, camera.GetViewMatrix(), glm::radians(camera.Zoom), (float) gameHeight/(float)gameWidth,  0.1f, 1000.0f, VAO, VAOSafeLane, lanesArray, newStart, car, truck, coin, tree);
        
        // Check if lanes generation needed
        if (penguin.getPenZ() < lanesArray[34].startPos){
            Utilities::addMoreLanes(lanesArray, newStart);
            penguin.setCurrentLaneIndex(4);
        }
        // Check for collisions
        collisionState=penguin.detectCollision(lanesArray);
        if (collisionState == carCollision) {
            gameOver = true;
        }
        frameCount++;
        if(frameCount >= penguin.getPenguinMode()->getModelAnimations()[0].second.size()){
            frameCount = 0;
        }
        graphicsUtilities.score(lanesArray,score, &penguin, engine);
        
        // Display score
        fonts.RenderText("Score: " + to_string(score), 25.0f, gameHeight - 60.0f, 0.8f, glm::vec3(1, 1, 0));
        
        if(gameOver){
            // Print lossing msg
            fonts.RenderText("You lost!", gameWidth/2.0f - 80, gameHeight/ 2.0f, 0.8f, glm::vec3(1, 1, 0));
            
            // Click enter to restart game
            glfwSwapBuffers(window);
            // Play gameover
            if(!gameOverSoundPlayed){
                engine->stopAllSounds();
                engine->play2D("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Sounds/GameOver.wav");
                gameOverSoundPlayed = true;
            }
            do {
                glfwPollEvents();
            } while((glfwGetKey(window, GLFW_KEY_ESCAPE)) != GLFW_PRESS);
            exitGame = true;
            
        }
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void RenderEverything(Shader &simpleDepthShader, glm::mat4 &lightProjection,glm::mat4  &lightView, glm::mat4 &lightSpaceMatrix,GLfloat &near_plane ,GLfloat &far_plane, GameScene &gameScene, Penguin &penguin,bool &movingForward,bool &movingBackward, bool &movingRight,bool &movingLeft, vector<lane> &lanesArray,GLuint &frameCount,GLfloat &deltaTime, GLuint &depthMapFBO, GLuint &depthMap, float &newStart, GraphicsUtilities &graphicsUtilities, Car &car, Car &truck, Coin &coin,Tree &tree, GLuint &VAO, GLuint  &VAOSafeLane){
    
    lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(ax, ay, az), glm::vec3(0.0, 1.0, 0.0));
    //Light Space Matrix has the view and the projection
    lightSpaceMatrix = lightProjection * lightView;
    // Now render scene from light's point of view
    simpleDepthShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    //Render The Penguin
    graphicsUtilities.do_movement(deltaTime, movingForward, movingBackward, movingRight, movingLeft);
    penguin.move(movingForward, movingBackward, movingRight, movingLeft,camera, deltaTime, newStart, lanesArray);
    penguin.Render(simpleDepthShader, frameCount, (movingForward || movingRight || movingLeft || movingBackward), deltaTime, lanesArray);
    //Render Gamescene(Lanes, Coins, Trees, Cars)
    gameScene.Render(simpleDepthShader, lanesArray, newStart, VAO, VAOSafeLane, car, truck, coin, tree);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
