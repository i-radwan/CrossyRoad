#include <cmath>
#include <fstream>
#include <SOIL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Utilities/utils.h"
#include "Utilities/graphics_utils.h"

#include "Model/PenModel.h"
#include "Models/GameScene.h"
#include "Models/Penguin.h"
#include "Models/Car.h"

using namespace std;

GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 4.0f, 5.0f));

const GLint gameWidth = 750;
const GLint gameHeight = 800;

GLFWwindow* window;

int main(int argc, const char * argv[]) {
    // initialize game
    GraphicsUtilities graphicsUtilities(&camera);
    if(0 != graphicsUtilities.initializeGameWindow(gameWidth, gameHeight, 3, 3, window)){
        return -1;
    }
    
    // Load SHADERS
    Shader mainShader("/Users/Samir/Documents/CrossyRoad/Shaders/shader.vs", "/Users/Samir/Documents/CrossyRoad/Shaders/shader.frag");
    Shader objShader("/Users/Samir/Documents/CrossyRoad/Shaders/objshader.vs", "/Users/Samir/Documents/CrossyRoad/Shaders/objshader.frag");
    Shader carShader("/Users/Samir/Documents/CrossyRoad/Shaders/carshader.vs", "/Users/Samir/Documents/CrossyRoad/Shaders/carshader.frag");
    
    // Load gamescene
    GameScene gameScene(mainShader);
    // Load MODELS
    Penguin penguin(objShader, "/Users/Samir/Documents/CrossyRoad/ModelsFiles/penguin.dae");
    penguin.setPenPosition(0, 1.4f, -3.0); // Set initial posisiton
    Car car(carShader, "/Users/Samir/Documents/CrossyRoad/ModelsFiles/Small_car_obj/Small car.obj");
    
    
    // Load the lane-cube into the fragment
    GLuint VBO, VAO;
    graphicsUtilities.bindCube(VBO, VAO);
    
    // Generate lanes
    vector<lane> lanesArray;
    Utilities::generateLanesAlgorithm(lanesArray);
    float newStart = -1.3;
    
    float*carsXPosition=new float[lanesArray.size()];
    for(int i = 0; i < lanesArray.size(); i++){
        carsXPosition[i] = 12;
    }
    int varSpeed=1;
    bool firstLanesSet = true;
    // Some variables for game logic go here::
    GLuint frameCount = 0;
    GLfloat laneAccumulatedSpeedAccelaration = 0;
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents(); // Check for keyboard or mouse events
        
        // Calculate frame time for smooth/quick movement on all devices
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        
        // Clear screen with certain background color
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        // Clear depth and color buffers with each iteration so the new values can be applied without blocking
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Sart drawing
        // Penguin drawing
        //Check for arrows movement ToDo fix diagonal movement
        bool movingForward = false, movingBackward = false, movingRight = false, movingLeft = false;
        graphicsUtilities.do_movement(deltaTime, movingForward, movingBackward, movingRight, movingLeft);
        penguin.move(movingForward, movingBackward, movingRight, movingLeft,camera, deltaTime, newStart);
        // penguin animations
        penguin.draw(camera.GetViewMatrix(), glm::radians(camera.Zoom), (float)gameHeight/gameWidth, 0.1f, 1000.0f, frameCount, (movingForward || movingRight || movingLeft || movingBackward));
        
        
        //Cars Objects drawing (ToDo, create car object and set its speed, start position, texture, ..etc)
        float carsZPos = ((firstLanesSet) ? (newStart-5) : newStart+0.5);
        
        for(GLuint i = ((firstLanesSet)? 5:0); i < lanesArray.size(); i++)
        {
            if(lanesArray[i].type == 1){
                car.draw(camera.GetViewMatrix(), glm::radians(camera.Zoom), (float) gameHeight/(float)gameWidth, 0.1f, 1000.0f, carsXPosition[i], 1.3, carsZPos, penguin.getPenZ(), penguin.getPenX());
            }
            // Z operations
            if((i < lanesArray.size()-1 && lanesArray[i].type == 1 && lanesArray[i + 1].type == 0) || (i < lanesArray.size()-1 && lanesArray[i].type == 0 && lanesArray[i + 1].type == 1)){
                carsZPos -=1.5;
            }
            else{
                carsZPos -= 1;
            }
            
            if(i < lanesArray.size()-1 && lanesArray[i].type == 1 && lanesArray[i+1].type == 1){
                carsZPos -= 1.1;
            }
        }
        
        // Set speed
        int sp =laneAccumulatedSpeedAccelaration;
        for(int i = 0; i < lanesArray.size(); i++)
        {
            // Rotate cars
            if(carsXPosition[i]<-25)
            {carsXPosition[i]=25;}
            
            if((varSpeed%2)==0)
            {
                carsXPosition[i]-=0.01;
            }
            if((varSpeed%3)==0)
            {
                carsXPosition[i]-=0.02;
            }
            if((varSpeed%5)==0)
            {
                carsXPosition[i]-=0.025;
            }
            carsXPosition[i]-= (sp)*0.01;
            sp++;
            varSpeed++;
        }
        
        // Draw the scene
        gameScene.draw(camera.GetViewMatrix(), glm::radians(camera.Zoom), (float) gameHeight/(float)gameWidth,  0.1f, 1000.0f, VAO, lanesArray, newStart, penguin.getPenZ(), penguin.getPenX());
        
        cout << lanesArray[39].startPos << " " << penguin.getPenZ() <<endl;
        cout << newStart << endl;
        // Check if lanes generation needed
        if (penguin.getPenZ() < lanesArray[34].startPos){
            Utilities::addMoreLanes(lanesArray, newStart, firstLanesSet, carsXPosition);
            laneAccumulatedSpeedAccelaration += 34;
        }
        frameCount++;
        if(frameCount >= penguin.getPenguinMode()->getModelAnimations()[0].second.size()){
            frameCount = 0;
        }
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

