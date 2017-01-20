/**
 @IAR:
 @ToDo:
 • Find the proper function for speed calculations
 • Deallocate BFS nodes
 • Fix penguin rotating while auto-running
 */
#include <cmath>
#include <fstream>
#include <map>
#include <queue>
#include <stack>
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
#include "Utilities/autorun.h"
using namespace std;

GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 4.0f, 5.0f));

long score = 0;
bool gameOver = false;
bool exitGame = false;
bool gameOverSoundPlayed= false;
GLFWwindow* window;

float lightX = -22, lightY = 35, lightZ = -30, lookAtX = 0,lookAtY = 0,lookAtZ = -25;
vector<Lane> lanesArray;
CollisionStatus collisionState;

//For Rendering the DepthMap (Debuging and testing)
GLuint quadVAO = 0;
GLuint quadVBO;

int main(int argc, const char * argv[]) {
    // Randomize game auto generation
    srand(time(NULL));
    // initialize game
    GraphicsUtilities graphicsUtilities(&camera);
    if(0 != graphicsUtilities.initializeGameWindow(Constants::gameWidth, Constants::gameHeight, 3, 3, window)){
        return -1;
    }
    
    // Setup autorun in case needed
    AutoRun* autoRun = new AutoRun(lanesArray, false);//Set 2nd Arg to true to enable autoRun
    // Load SHADERS
    Shader sceneShader("/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Shaders/shader.vs", "/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Shaders/shader.frag");
    Shader materialShader("/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Shaders/objshader.vs", "/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Shaders/objshader.frag");
    Shader textureShader("/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Shaders/shadow_mapping.vs", "/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Shaders/shadow_mapping.frag");
    Shader fontShader("/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Shaders/font.vs", "/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Shaders/font.frag");
    Shader simpleDepthShader("/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Shaders/shadow_mapping_depth.vs", "/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Shaders/shadow_mapping_depth.frag");
    
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
    Penguin penguin(materialShader, "/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/ModelsFiles/penguin.dae", camera);
    penguin.setPenPosition(0, penguin.constantPenY, -3.3f); // Set initial posisiton
    Car car(textureShader, "/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/ModelsFiles/Small_car_obj/Small car.obj");
    Car truck(textureShader, "/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/ModelsFiles/cubus_deutz_rund/tlf16_rund.obj");
    Coin coin(textureShader ,"/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/ModelsFiles/coin/Gems/diamond_orange.obj");
    Tree tree(textureShader, "/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/ModelsFiles/Tree/tree_4.obj");
    
    // Generate lanes
    Utilities::generateLanesAlgorithm(lanesArray);
    
    // Some variables for game logic go here::
    GLuint frameCount = 0;
    
    // Loading fonts
    Fonts fonts(fontShader, Constants::gameWidth, Constants::gameHeight, "/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/ModelsFiles/zorque.ttf");
    
    // Load the Lane-cube into the fragment
    GLuint VBO, VAO;
    graphicsUtilities.bindCube(VBO, VAO);
    GLuint VBOSafeLane, VAOSafeLane;
    graphicsUtilities.bindCube(VBOSafeLane, VAOSafeLane, true);
    
    //DepthMap initialization
    GLuint depthMapFBO;
    //depth texture
    GLuint depthMap;
    //DeapthMap Buffer Bindings
    graphicsUtilities.DepthMapInitialization(depthMapFBO, depthMap, Constants::SHADOW_WIDTH, Constants::SHADOW_HEIGHT);
    
    //depthMapInitializtion(depthMap, depthMapFBO,SHADOW_WIDTH,SHADOW_HEIGHT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    //play Background Music
    
    engine->play2D("/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Sounds/CarHornBase.wav", true);
    engine->play2D("/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Sounds/180156__klankbeeld__traffic-horns-city-nervous-busy.wav", true);
    engine->play2D("/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Sounds/MagicMatch_GameTheme.ogg", true);
    
    /*****************
     AUTORUN
     *****************/
    if(autoRun->isAutoRunEnabled){
        autoRun->currentbfsNode->laneZIndex = 2;
        autoRun->currentbfsNode->nodeX = 9;
        autoRun->targetPenX = penguin.getPenX();
        autoRun->testPen = &penguin;
    }
    stack<int> moves;
    /*****************
     END AUTORUN
     *****************/
    while(!glfwWindowShouldClose(window) && !exitGame)
    {
        glfwPollEvents(); // Check for keyboard or mouse events
        
        // Calculate frame time for smooth/quick movement on all devices
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        
        //Render depth of scene to texture (from light's perspective)
        //Get light projection view matrix
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        GLfloat near_plane = -1.0f, far_plane = 60.0f;
        
        //Penguin Movement Attributes
        bool movingForward = false, movingBackward = false, movingRight = false, movingLeft = false;
        /**********
         AUTO RUN
         **********/
        if(autoRun->isAutoRunEnabled) {
            if(abs(autoRun->targetPenX- penguin.getPenX())>0.15){
                if(autoRun->penmove == 3){
                    movingRight = true;
                } else if(autoRun->penmove == 4){
                    movingLeft = true;
                }
            } else {
                if(moves.empty()){ // Penguin stopped and no moves in stack
                    bfsNode* finalNode = autoRun->autoRunAlgo();
                    autoRun->penmove = 0;
                    while(finalNode != 0){
                        moves.push(finalNode->move);
                        finalNode = finalNode->parent;
                    }
                    autoRun->penmove = moves.top();
                    moves.pop();
                    
                    if(!autoRun->penmove){
                        movingForward = movingBackward = movingLeft = movingRight = false;
                    }
                    autoRun->currentbfsNode->parent = 0;
                    autoRun->currentbfsNode->next = 0;
                    autoRun->currentbfsNode->move = 0;
                } else if(!moves.empty() && (!(penguin.movingLeft || penguin.movingRight || penguin.movingForwad || penguin.movingBackward|| penguin.isMoving))){ // Penguin finished current move and waiting to fetch the next
                    autoRun->penmove = moves.top();
                    moves.pop();
                }
                if(autoRun->penmove == 1){
                    movingForward = true;
                } else if(autoRun->penmove == 2){
                    movingBackward = true;
                } else if(autoRun->penmove == 3){
                    movingRight = true;
                    autoRun->targetPenX = penguin.getPenX() + 1;
                } else if(autoRun->penmove == 4){
                    movingLeft = true;
                    autoRun->targetPenX = penguin.getPenX() - 1;
                }
                autoRun->penmove = 0;
            }
        }
        
        /**************************
         END AUTORUN
         *************************/
        //Call Render Everything functuion
        graphicsUtilities.renderEverything(simpleDepthShader, lightProjection, lightView, lightSpaceMatrix, near_plane, far_plane, gameScene, penguin, movingForward, movingBackward, movingRight, movingLeft, lanesArray, frameCount, deltaTime, depthMapFBO, depthMap, graphicsUtilities, car, truck, coin, tree, VAO, VAOSafeLane, glm::vec3(lightX, lightY, lightZ), lookAtX, lookAtY, lookAtZ, camera);
        
        //Update the Light Pos And the Light Look at
        lightZ = penguin.getPenZ() + 8.3;
        lookAtZ = penguin.getPenZ() + 36.3;
        
        // Clear screen with certain background color
        glViewport(0, 0, Constants::gameWidth*2, Constants::gameHeight*2);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        // Clear depth and color buffers with each iteration so the new values can be applied without blocking
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        // Sart drawing
        
        // Penguin drawing
        if(!autoRun->isAutoRunEnabled){
            graphicsUtilities.do_movement(deltaTime, movingForward, movingBackward, movingRight, movingLeft);
        }
        penguin.move(movingForward, movingBackward, movingRight, movingLeft,camera, deltaTime, lanesArray);
        
        // penguin drawing
        penguin.draw(camera.GetViewMatrix(), glm::radians(camera.Zoom), (float)Constants::gameHeight/Constants::gameWidth, 0.1f, 100.0f, frameCount, (movingForward || movingRight || movingLeft || movingBackward), deltaTime, lanesArray);
        
        // Draw the scene (lanes + cars + Diamonds + trees)
        gameScene.draw(lightSpaceMatrix, depthMap, camera, glm::vec3(lightX, lightY, lightZ), shadows, camera.GetViewMatrix(), glm::radians(camera.Zoom), (float) Constants::gameHeight/(float)Constants::gameWidth,  0.1f, 100.0f, VAO, VAOSafeLane, lanesArray, car, truck, coin, tree);
        
        // Check if lanes generation needed
        if (penguin.getCurrentLane() > 34){
            Utilities::addMoreLanes(lanesArray);
            penguin.setCurrentLaneIndex(3);
            penguin.setAdjacentLaneIndex(4);
            /******************
             AUTORUN
             *****************/
            if(autoRun->isAutoRunEnabled){
                autoRun->currentbfsNode->laneZIndex = 3;
            }
        }
        // Check for collisions
        collisionState=penguin.detectCollision(lanesArray);
        if (collisionState == CAR_COLLISION) {
            gameOver = true;
        }
        frameCount++;
        if(frameCount >= penguin.getPenguinMode()->getModelAnimations()[0].second.size()){
            frameCount = 0;
        }
        graphicsUtilities.score(lanesArray,score, &penguin, engine);
        
        // Display score
        fonts.RenderText("Score: " + to_string(score), 25.0f, Constants::gameHeight - 60.0f, 0.8f, glm::vec3(1, 1, 0));
        
        if(gameOver) {
            cout << "CURRENT LANE "  << penguin.getCurrentLane() << " XPOS:: " << lanesArray[penguin.getCurrentLane()].getLaneCarXPosition() << " AND SPEED::  "<<  lanesArray[penguin.getCurrentLane()].getLaneCarSpeed() << " PengX :: " << penguin.getPenX() <<endl;
            
            // Print lossing msg
            fonts.RenderText("You lost!", Constants::gameWidth/2.0f - 80, Constants::gameHeight/ 2.0f, 0.8f, glm::vec3(1, 1, 0));
            
            // Click enter to restart game
            glfwSwapBuffers(window);
            // Play gameover
            if(!gameOverSoundPlayed){
                engine->stopAllSounds();
                engine->play2D("/Users/ibrahimradwan/Development/College/CrossyRoad/opengl/opengl/Sounds/GameOver.wav");
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

