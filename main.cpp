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
collisionStatus collisionState;

//For Rendering the DepthMap (Debuging and testing)
GLuint quadVAO = 0;
GLuint quadVBO;

/**************
 TESTING
 **************/
int iX = 0, jZ = 0;
int penmove;
float targetPenX ;
bool isLookingForPathOrMoving = false;

Penguin* testPen;
struct bfsNode{
    bfsNode* next;
    bfsNode* parent;
    int nodeX = 0, laneZIndex = 0;
    int move = 0;
    int accumulatedFramesCount =0;
    bfsNode(){
        next = parent = 0;
    }
};
bfsNode* currentbfsNode;
bfsNode* test(bfsNode* currentbfsNode){
    isLookingForPathOrMoving = true;
    vector<vector<int> > visited(50, std::vector<int> ( 17, 0)); // 50 * 17 vector to hold visited
    int requZ = currentbfsNode->laneZIndex + 6;
    
    visited[currentbfsNode->laneZIndex][currentbfsNode->nodeX] = 1;
    if(currentbfsNode->laneZIndex != testPen->getCurrentLane()){
        cout <<"ERROR " <<  currentbfsNode->laneZIndex  << " " << testPen->getCurrentLane()<<endl;
    }
    queue<bfsNode*> q;
    q.push(currentbfsNode);
    bfsNode* tmp;
    
    float backupX = testPen->getPenX();
    float backupZ = testPen->getPenZ();
    int backupLaneIndex = testPen->getCurrentLane();
    currentbfsNode->accumulatedFramesCount=0;
    while(!q.empty()){
        tmp = q.front();
        q.pop();
        if (tmp->laneZIndex == requZ) { // check if reached required
            isLookingForPathOrMoving= false;
            testPen->setPenZ(backupZ);
            testPen->setPenX(backupX);
            testPen->setCurrentLaneIndex(backupLaneIndex);
            return tmp;
        }
        Lane nextLane, previousLane, currentLane;
        nextLane = lanesArray[tmp->laneZIndex +1];
        currentLane = lanesArray[tmp->laneZIndex];
        previousLane = lanesArray[tmp->laneZIndex -1];
        
        testPen->setPenZ(nextLane.laneZPos);
        testPen->setPenX(tmp->nodeX-9);
        testPen->setCurrentLaneIndex(tmp->laneZIndex +1);
        float framesToBeAdded = 0;

        if(nextLane.getLaneCarXPosition() > tmp->nodeX -9+2){
            if(currentLane.type == SAFE_LANE && nextLane.type == SAFE_LANE)
                framesToBeAdded = 25.0;
            else if (currentLane.type == SAFE_LANE && nextLane.type == NORMAL_LANE ||
                     currentLane.type == NORMAL_LANE && nextLane.type == SAFE_LANE)
                framesToBeAdded = 38.0;
            else if (currentLane.type == NORMAL_LANE && nextLane.type == NORMAL_LANE)
                framesToBeAdded = 52.0;

        }else { // before
            if(currentLane.type == SAFE_LANE && nextLane.type == SAFE_LANE)
                framesToBeAdded = 25.0/3;
            else if (currentLane.type == SAFE_LANE && nextLane.type == NORMAL_LANE ||
                     currentLane.type == NORMAL_LANE && nextLane.type == SAFE_LANE)
                framesToBeAdded = 38.0/3;
            else if (currentLane.type == NORMAL_LANE && nextLane.type == NORMAL_LANE)
                framesToBeAdded = 52.0/3;
            
        }
        float horzFramesCount = 0;
            if(currentLane.getLaneCarXPosition() > tmp->nodeX -9){
                horzFramesCount = 40.0;
            } else { // before
               horzFramesCount = -40.0/2;
            }
        if(tmp->laneZIndex+1 < 50 && !visited[tmp->laneZIndex+1][tmp->nodeX] &&
           testPen->detectCollisionWithAutoRun(lanesArray, framesToBeAdded) == noCollision
           ){
            
            if(!(lanesArray[tmp->laneZIndex +1].type == SAFE_LANE &&  abs(lanesArray[tmp->laneZIndex +1].treeXpos - (tmp->nodeX-9)) < 1.3)){
                
                bfsNode* newNode = new bfsNode;
                newNode->nodeX = tmp->nodeX;
                newNode->laneZIndex = tmp->laneZIndex+1;
                newNode->move = 1;
                newNode->parent = tmp;
                newNode->accumulatedFramesCount = tmp->accumulatedFramesCount+framesToBeAdded;
                tmp->next = newNode;
                q.push(newNode);
                visited[tmp->laneZIndex+1][tmp->nodeX] = 1;
            }}
        testPen->setCurrentLaneIndex(tmp->laneZIndex);
        testPen->setPenZ(currentLane.laneZPos);
        testPen->setPenX(tmp->nodeX-7);
        
        if(tmp->nodeX+1 < 17 &&!visited[tmp->laneZIndex][tmp->nodeX+1] &&
           testPen->detectCollisionWithAutoRun(lanesArray, horzFramesCount) == noCollision){
            if((lanesArray[tmp->laneZIndex].type == LaneType::SAFE_LANE
                && (abs((tmp->nodeX-9)+1  - lanesArray[tmp->laneZIndex].treeXpos) > 1.3) || ((tmp->nodeX-9)+1 > lanesArray[tmp->laneZIndex].treeXpos)) || lanesArray[tmp->laneZIndex].type == LaneType::NORMAL_LANE){
                
                bfsNode* newNode = new bfsNode;
                newNode->nodeX = tmp->nodeX+1;
                newNode->laneZIndex = tmp->laneZIndex;
                newNode->parent = tmp;
                newNode->move = 3;
                newNode->accumulatedFramesCount = tmp->accumulatedFramesCount+20;
                tmp->next = newNode;
                q.push(newNode);
                visited[tmp->laneZIndex][tmp->nodeX+1] = 1;
            }
        }
        testPen->setCurrentLaneIndex(tmp->laneZIndex);
        testPen->setPenZ(currentLane.laneZPos);
        testPen->setPenX(tmp->nodeX-9);
        if(tmp->nodeX-1 > 0 &&!visited[tmp->laneZIndex][tmp->nodeX-1]&&
           testPen->detectCollisionWithAutoRun(lanesArray, horzFramesCount) == noCollision){
            
            if((lanesArray[tmp->laneZIndex].type == LaneType::SAFE_LANE
                && (abs((tmp->nodeX-9)-1  - lanesArray[tmp->laneZIndex].treeXpos) > 1.3) || ((tmp->nodeX-9)-1 < lanesArray[tmp->laneZIndex].treeXpos)) || lanesArray[tmp->laneZIndex].type == LaneType::NORMAL_LANE){
                bfsNode* newNode = new bfsNode;
                newNode->nodeX = tmp->nodeX-1;
                newNode->laneZIndex = tmp->laneZIndex;
                newNode->parent = tmp;
                newNode->move = 4;
                tmp->next = newNode;
                newNode->accumulatedFramesCount = tmp->accumulatedFramesCount+20;
                q.push(newNode);
                visited[tmp->laneZIndex][tmp->nodeX-1] = 1;
            }
        }
        
        testPen->setPenZ(previousLane.laneZPos);
        testPen->setCurrentLaneIndex(tmp->laneZIndex - 1);
        testPen->setPenX((tmp->nodeX-9));
        
        if(tmp->laneZIndex-1 > 0 && !visited[tmp->laneZIndex-1][tmp->nodeX] &&
           testPen->detectCollisionWithAutoRun(lanesArray, framesToBeAdded) == noCollision){
            
            if(!(lanesArray[tmp->laneZIndex -1].type == SAFE_LANE &&  abs(lanesArray[tmp->laneZIndex -1].treeXpos - (tmp->nodeX-9)) < 1.3)){
                
                bfsNode* newNode = new bfsNode;
                newNode->nodeX = tmp->nodeX;
                newNode->laneZIndex = tmp->laneZIndex-1;
                newNode->parent = tmp;
                newNode->move = 2;
                newNode->accumulatedFramesCount = tmp->accumulatedFramesCount+framesToBeAdded;
                tmp->next = newNode;
                q.push(newNode);
                
                visited[tmp->laneZIndex-1][tmp->nodeX] = 1;
            }
        }
    }
    testPen->setPenZ(backupZ);
    testPen->setPenX(backupX);
    testPen->setCurrentLaneIndex(backupLaneIndex);
    isLookingForPathOrMoving= false;
    return 0;
}
int main(int argc, const char * argv[]) {
    srand(time(NULL));
    // initialize game
    GraphicsUtilities graphicsUtilities(&camera);
    if(0 != graphicsUtilities.initializeGameWindow(Constants::gameWidth, Constants::gameHeight, 3, 3, window)){
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
    penguin.setPenPosition(0, penguin.constantPenY, -3.3f); // Set initial posisiton
    Car car(textureShader, "/Users/ibrahimradwan/Desktop/Small_car_obj/Small car.obj");
    Car truck(textureShader, "/Users/ibrahimradwan/Desktop/cubus_deutz_rund/tlf16_rund.obj");
    Coin coin(textureShader ,"/Users/ibrahimradwan/Desktop/coin/Gems/diamond_orange.obj");
    Tree tree(textureShader, "/Users/ibrahimradwan/Desktop/Tree/tree_4.obj");
    
    // Generate lanes
    Utilities::generateLanesAlgorithm(lanesArray);
    
    // Some variables for game logic go here::
    GLuint frameCount = 0;
    
    // Loading fonts
    Fonts fonts(fontShader, Constants::gameWidth, Constants::gameHeight, "/Users/ibrahimradwan/Desktop/zorque.ttf");
    
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
    
    engine->play2D("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Sounds/CarHornBase.wav", true);
    engine->play2D("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Sounds/180156__klankbeeld__traffic-horns-city-nervous-busy.wav", true);
    engine->play2D("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Sounds/MagicMatch_GameTheme.ogg", true);
    
    /*****************
     TESTING
     *****************/
    currentbfsNode = new bfsNode();//initial point
    currentbfsNode->laneZIndex = 2;
    currentbfsNode->nodeX = 9;
    targetPenX = penguin.getPenX();
    testPen = &penguin;
    /*****************
     END TESTING
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
         TESTING
         **********/
        
        if(!(penguin.movingLeft || penguin.movingRight || penguin.movingForwad || penguin.movingBackward|| penguin.isMoving)){ // Penguin isn't moving
            if(abs(targetPenX- penguin.getPenX())>0.15){
                if(penmove == 3){
                    movingRight = true;
                } else if(penmove == 4){
                    movingLeft = true;
                }
            }
            if(abs(targetPenX- penguin.getPenX())<0.15){
                
                bfsNode* finalNode = test(currentbfsNode);
                while(finalNode != 0){
                    if(finalNode->move){
                        if(finalNode->parent->move == 0){
                            currentbfsNode = finalNode;
                            penmove = finalNode->move;
                        }
                    }
                    finalNode = finalNode->parent;
                }
                currentbfsNode->parent = 0;
                currentbfsNode->next = 0;
                currentbfsNode->move = 0;
            }
            if(abs(targetPenX- penguin.getPenX())<0.15){
                if(penmove == 1){
                    movingForward = true;
                } else if(penmove == 2){
                    movingBackward = true;
                } else if(penmove == 3){
                    movingRight = true;
                    targetPenX = penguin.getPenX() + 1;
                } else if(penmove == 4){
                    movingLeft = true;
                    targetPenX = penguin.getPenX() - 1;
                }
                
            }
        }
        /**************************
         TEST END
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
        graphicsUtilities.do_movement(deltaTime, movingForward, movingBackward, movingRight, movingLeft);
        penguin.move(movingForward, movingBackward, movingRight, movingLeft,camera, deltaTime, lanesArray);
        // penguin drawing
        penguin.draw(camera.GetViewMatrix(), glm::radians(camera.Zoom), (float)Constants::gameHeight/Constants::gameWidth, 0.1f, 1000.0f, frameCount, (movingForward || movingRight || movingLeft || movingBackward), deltaTime, lanesArray);
        
        // Draw the scene (lanes + cars + Diamonds + trees)
        gameScene.draw(lightSpaceMatrix, depthMap, camera, glm::vec3(lightX, lightY, lightZ), shadows, camera.GetViewMatrix(), glm::radians(camera.Zoom), (float) Constants::gameHeight/(float)Constants::gameWidth,  0.1f, 1000.0f, VAO, VAOSafeLane, lanesArray, car, truck, coin, tree);
        
        // Check if lanes generation needed
        if (penguin.getCurrentLane() > 34){
            Utilities::addMoreLanes(lanesArray);
            penguin.setCurrentLaneIndex(3);
            penguin.setAdjacentLaneIndex(4);
            /******************
             TESTING
             *****************/
            currentbfsNode->laneZIndex = 3;
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
        fonts.RenderText("Score: " + to_string(score), 25.0f, Constants::gameHeight - 60.0f, 0.8f, glm::vec3(1, 1, 0));
        
        cout << "CURRENT LANE CAR LOCATION AND SPEED "  << penguin.getCurrentLane() << " " << lanesArray[penguin.getCurrentLane()].getLaneCarXPosition() << " AND POST "<<  lanesArray[penguin.getCurrentLane()].getLaneCarSpeed() <<endl;

        if(gameOver) {
            cout << "DEAD ON " << penguin.getCurrentLane() << " AND X " << penguin.getPenX()<<endl;
            // Print lossing msg
            fonts.RenderText("You lost!", Constants::gameWidth/2.0f - 80, Constants::gameHeight/ 2.0f, 0.8f, glm::vec3(1, 1, 0));
            
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

