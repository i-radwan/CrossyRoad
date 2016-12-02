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

#include "Model/Model.h"
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
    Shader mainShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/shader.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/shader.frag");
    Shader objShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/objshader.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/objshader.frag");
    Shader carShader("/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/carshader.vs", "/Users/ibrahimradwan/Desktop/Graphics/CrossyRoad/opengl/opengl/Shaders/carshader.frag");
    
    // Load gamescene
    GameScene gameScene(mainShader);
    // Load MODELS
    Penguin penguin(objShader, "/Users/ibrahimradwan/Desktop/untitled2.obj");
    penguin.setPenPosition(0, 1.4f, -3.0); // Set initial posisiton
    Car car(carShader, "/Users/ibrahimradwan/Desktop/Small_car_obj/Small car.obj");
    
    
    // Load the lanes cube into the fragment
    GLuint VBO, VAO;
    graphicsUtilities.bindCube(VBO, VAO);
    
    // Generate lanes
    vector<int> lanes;
    Utilities::generateLanesAlgorithm(lanes);
    
    // Some variables for game logic go here::
    
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents(); // Check for keyboard or mouse events
        
        // Calculate frame time for smooth/quick movement on all devices
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if lanes generation needed
        if (camera.Position.z < (int)(-1*lanes.size())){
            Utilities::addMoreLanes(lanes);
        }
        
        // Clear screen with certain background color
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        // Clear depth and color buffers with each iteration so the new values can be applied without blocking
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Sart drawing
        // Penguin drawing
        //Check for arrows movement
        if(graphicsUtilities.do_movement(deltaTime) == FORWARD_MOVEMENT){
            penguin.setPenZ(penguin.getPenZ() - 0.1f);
        } else if (graphicsUtilities.do_movement(deltaTime) == BACKWARD_MOVEMENT){
            penguin.setPenZ(penguin.getPenZ() + 0.1f);
        } else if (graphicsUtilities.do_movement(deltaTime) == RIGHT_MOVEMENT){
            penguin.setPenX(penguin.getPenX() + 0.1f);
        } else if (graphicsUtilities.do_movement(deltaTime) == LEFT_MOVEMENT){
            penguin.setPenX(penguin.getPenX() - 0.1f);
        }
        penguin.draw(camera.GetViewMatrix(), glm::radians(camera.Zoom), (float)gameHeight/gameWidth, 0.1f, 1000.0f);
        
        //Cars Objects drawing (ToDo, create car object and set its speed, start position, texture, ..etc)
        float carsZPos = 0;
        for(GLuint i = 0; i < lanes.size(); i++)
        {
            carsZPos -=1;
            if(lanes[i] == 1){
                car.draw(camera.GetViewMatrix(), glm::radians(camera.Zoom), (float) gameHeight/(float)gameWidth, 0.1f, 1000.0f, 0, 0.9f, carsZPos);
            }
            if(i < lanes.size()-1 && lanes[i] == 1 && lanes[i+1] == 1){
                carsZPos -= 1.2;
            }
            
        }
        
        // Draw the scene
        gameScene.draw(camera.GetViewMatrix(), glm::radians(camera.Zoom), (float) gameHeight/(float)gameWidth,  0.1f, 1000.0f, VAO, lanes);
        
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}


