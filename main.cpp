#include <cmath>
#include <fstream>
#include <map>

#include <SOIL.h>
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
using namespace std;

GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 4.0f, 5.0f));

const GLint gameWidth = 750;
const GLint gameHeight = 800;
GLint gameScore = 0;

GLFWwindow* window;

int main(int argc, const char * argv[]) {
    
    // initialize game
    GraphicsUtilities graphicsUtilities(&camera);
    if(0 != graphicsUtilities.initializeGameWindow(gameWidth, gameHeight, 3, 3, window)){
        return -1;
    }

    // Load SHADERS
    Shader mainShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/shader.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/shader.frag");
    Shader objShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/objshader.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/objshader.frag");
    Shader carShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/carshader.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/carshader.frag");
    Shader fontShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/font.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/font.frag");
    
    // Load gamescene
    GameScene gameScene(mainShader);
    // Load MODELS
    Penguin penguin(objShader, "/Users/ibrahimradwan/Desktop/penguin.dae");
    penguin.setPenPosition(0, 1.4f, -3.0); // Set initial posisiton
    Car car(carShader, "/Users/ibrahimradwan/Desktop/Small_car_obj/Small car.obj");
    
    
    // Generate lanes
    vector<lane> lanesArray;
    Utilities::generateLanesAlgorithm(lanesArray);
    
    
    float newStart = -1.3;
    // To remove
    float*carsXPosition=new float[lanesArray.size()];
    for(int i = 0; i < lanesArray.size(); i++){
        carsXPosition[i] = rand() % 18 + 12;
    }
    int varSpeed=1;
    bool firstLanesSet = true;
    // Some variables for game logic go here::
    GLuint frameCount = 0;
    GLfloat laneAccumulatedSpeedAccelaration = 0;
    
    // Loading fonts
    Fonts fonts(fontShader, gameWidth, gameHeight, "/Users/ibrahimradwan/Desktop/zorque.ttf");
    
    // Load the lane-cube into the fragment
    GLuint VBO, VAO;
    graphicsUtilities.bindCube(VBO, VAO);
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
        bool movingForward = false, movingBackward = false, movingRight = false, movingLeft = false;
        graphicsUtilities.do_movement(deltaTime, movingForward, movingBackward, movingRight, movingLeft);
        penguin.move(movingForward, movingBackward, movingRight, movingLeft,camera, deltaTime, newStart);
        // penguin animations
        penguin.draw(camera.GetViewMatrix(), glm::radians(camera.Zoom), (float)gameHeight/gameWidth, 0.1f, 1000.0f, frameCount, (movingForward || movingRight || movingLeft || movingBackward));
        
        // Draw the scene (lanes + cars)
        gameScene.draw(camera.GetViewMatrix(), glm::radians(camera.Zoom), (float) gameHeight/(float)gameWidth,  0.1f, 1000.0f, VAO, lanesArray, newStart, car);
        
        // Check if lanes generation needed
        if (penguin.getPenZ() < lanesArray[34].startPos){
            Utilities::addMoreLanes(lanesArray, newStart, firstLanesSet, carsXPosition);
            laneAccumulatedSpeedAccelaration += 34;
        }
            
        frameCount++;
        if(frameCount >= penguin.getPenguinMode()->getModelAnimations()[0].second.size()){
            frameCount = 0;
        }
        // Todo @Abdo (proper score counting)
        if(frameCount % 60 == 0){
            gameScore++;
        }
        // Display score
        fonts.RenderText("Score: " + to_string(gameScore), 25.0f, gameHeight - 60.0f, 0.8f, glm::vec3(1, 1, 0));
        
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
