#ifndef PENGUIN
#define PENGUIN

#include "../Utilities/shaders.h"
#include "../Model/PenModel.h"
#include <glm/glm.hpp>
#include <string>
using namespace std;
enum collisionStatus {
    carCollision, coinCollision, noCollision
};
class Penguin {
    
public:
    Penguin(Shader shader, GLchar* modelLink, Camera& c) :
    shader(shader), camera(c) {
        penguinModel = new PenModel(modelLink);
    }
    bool double_equals(double a, double b, double epsilon = 0.1) {
        return std::abs(a - b) < epsilon;
    }
    
    void movePenguinTowardsTarget(float deltaTime, vector<lane> &lanesArray) {
        if (isMoving) {
            if (double_equals(this->targetZ, this->penZ)) {
                isMoving = false;
                if (lanesArray[this->currentLaneIndex].type == 0) {
                    this->penY = constantPenY;
                } else
                    this->penY = constantPenY - 0.2;
                // Handle continous click
                if (this->movingForwad) {
                    this->isMoving = true; // Set penguin status to moving
                    this->targetZ = getNextLaneZ(lanesArray);
                    this->initalZ = this->penZ;
                }
            }
            if (!double_equals(this->targetZ, this->penZ)
                && this->targetZ < this->penZ) {
                // Jump the penguin
                if (this->targetZ - this->penZ > (this->penZ - this->initalZ)){
                    this->penY -= 0.07f;
                    if(!this->isMovedToAdjacentLane){
                        this->isMovedToAdjacentLane = true;
                        this->currentLaneIndex = this->adjacentLaneIndex;
                    }
                }
                else
                    this->penY += 0.07f;
                // Move the penguin
                this->penZ -= penguinSpeed;
                // Move camera
                glm::vec3 v(0, 0, -penguinSpeed);
                camera.updateCameraPos(v);
            } else if (!double_equals(this->targetZ, this->penZ)
                       && this->targetZ > this->penZ) {
                if (this->targetZ - this->penZ > (this->penZ - this->initalZ))
                    this->penY += 0.15f;
                else{
                    if(!this->isMovedToAdjacentLane){
                        this->isMovedToAdjacentLane = true;
                        this->currentLaneIndex = this->adjacentLaneIndex;
                    }
                    this->penY -= 0.07f;
                }
                this->penZ += penguinSpeed;
                glm::vec3 v(0, 0, penguinSpeed);
                camera.updateCameraPos(v);
            }
        }
    }
    void draw(glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio,
              float near, float far, GLuint frameCount, bool moving,
              float deltaTime, vector<lane> &lanesArray) {
        movePenguinTowardsTarget(deltaTime, lanesArray);
        shader.Use();
        glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
        GLint lightColorLoc = glGetUniformLocation(shader.Program,
                                                   "lightColor");
        glUniform3f(lightColorLoc, lightCol.x, lightCol.y, lightCol.z);
        
        glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near,
                                                   far);
        glm::mat4 objmodel;
        objmodel = glm::translate(objmodel,
                                  glm::vec3(this->penX, this->penY, this->penZ));
        if (this->movingRight) {
            if (!this->movingBackward)
                objmodel = glm::rotate(objmodel, glm::radians(-80.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
            else
                objmodel = glm::rotate(objmodel, glm::radians(80.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
        } else if (this->movingLeft) {
            if (!this->movingBackward)
                objmodel = glm::rotate(objmodel, glm::radians(80.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
            else
                objmodel = glm::rotate(objmodel, glm::radians(-80.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
        } else {
            objmodel = glm::rotate(objmodel, glm::radians(-20.0f),
                                   glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if ((!this->movingBackward || (this->isMoving && this->initalZ >= this->targetZ)) && !(this->isMoving && this->initalZ <= this->targetZ)) {
            objmodel = glm::rotate(objmodel, glm::radians(180.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
        }
        objmodel = glm::scale(objmodel, glm::vec3(0.13f, 0.13f, 0.13f));
        GLint objmodelLoc = glGetUniformLocation(shader.Program, "model");
        GLint objviewLoc = glGetUniformLocation(shader.Program, "view");
        GLint objprojectionLoc = glGetUniformLocation(shader.Program,
                                                      "projection");
        glUniformMatrix4fv(objviewLoc, 1, GL_FALSE,
                           glm::value_ptr(cameraViewMat));
        glUniformMatrix4fv(objprojectionLoc, 1, GL_FALSE,
                           glm::value_ptr(objprojection));
        glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
        penguinModel->Draw(shader, frameCount, objmodel,
                           moving || this->isMoving);
    }
    
    void Render(Shader& depthShader ,GLuint frameCount, bool moving,float deltaTime, vector<lane> &lanesArray){
        movePenguinTowardsTarget(deltaTime, lanesArray);
        //We are using the simpleDepthShader used in the Render Initialization Function
        glm::mat4 objmodel;
        objmodel = glm::translate(objmodel,
                                  glm::vec3(this->penX, this->penY, this->penZ));
        if (this->movingRight) {
            if (!this->movingBackward)
                objmodel = glm::rotate(objmodel, glm::radians(-80.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
            else
                objmodel = glm::rotate(objmodel, glm::radians(80.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
        } else if (this->movingLeft) {
            if (!this->movingBackward)
                objmodel = glm::rotate(objmodel, glm::radians(80.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
            else
                objmodel = glm::rotate(objmodel, glm::radians(-80.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
        } else {
            objmodel = glm::rotate(objmodel, glm::radians(-20.0f),
                                   glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if ((!this->movingBackward || (this->isMoving && this->initalZ >= this->targetZ)) && !(this->isMoving && this->initalZ <= this->targetZ)) {
            objmodel = glm::rotate(objmodel, glm::radians(180.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
        }
        objmodel = glm::scale(objmodel, glm::vec3(0.13f, 0.13f, 0.13f));
        GLint objmodelLoc = glGetUniformLocation(depthShader.Program, "model");
        
        glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
        //Render the Penguin model
        penguinModel->Render(depthShader, frameCount, objmodel,
                             moving || this->isMoving);
    }
    
    
    float getNextLaneZ(vector<lane> &lanesArray) {
        for (int i = 0; i < lanesArray.size(); i++) {
            if (double_equals(lanesArray[i].laneZPos, this->penZ)) {
                this->adjacentLaneIndex = i+1;
                this->isMovedToAdjacentLane = false;
                return lanesArray[i + 1].laneZPos;
            }
        }
        return this->penZ;
    }
    float getPreviousLaneZ(vector<lane> &lanesArray) {
        for (int i = 0; i < lanesArray.size(); i++) {
            if (i > 0 && double_equals(lanesArray[i].laneZPos, this->penZ)) {
                this->adjacentLaneIndex = i-1;
                this->isMovedToAdjacentLane = false;
                return lanesArray[i - 1].laneZPos;
            }
        }
        return this->penZ;
    }
    void move(bool movingForward, bool movingBackward, bool movingRight,
              bool movingLeft, Camera &camera, GLfloat deltaTime,
              vector<lane> &lanesArray) {
        deltaTime *= 100;
        this->movingForwad = movingForward;
        this->movingRight = movingRight;
        this->movingLeft = movingLeft;
        this->movingBackward = movingBackward;
        if (movingForward && !isMoving) {
            this->isMoving = true; // Set penguin status to moving
            this->targetZ = getNextLaneZ(lanesArray);
            this->initalZ = this->penZ;
        }
        if (movingBackward && !isMoving) {
            if (getPenZ() <= lanesArray[0].laneZPos - 3) {
                this->isMoving = true; // Set penguin status to moving
                this->targetZ = getPreviousLaneZ(lanesArray);
                this->initalZ = this->penZ;
            }
        }
        if (movingRight && getPenX() < 9.5) {
            setPenX(getPenX() + penguinSpeed * deltaTime);
            
        }
        if (movingLeft && getPenX() > -7) {
            setPenX(getPenX() - penguinSpeed * deltaTime);
        }
    }
    collisionStatus detectCollision(vector<lane> &lanesArray) {
        bool carCollided = false;
        bool coinCollided = false;
        float penRightPos = getPenX() + (1.492 * 0.13);
        float penLeftPos = getPenX() - (1.492 * 0.13);
        float carRightPos, carLeftPos;
        float coinRightPos, coinLeftPos;
        //collision with coins
        if (lanesArray[getCurrentLane()].hasCoin) {
            coinRightPos = lanesArray[getCurrentLane()].coinXPosition + 0.4;
            coinLeftPos = lanesArray[getCurrentLane()].coinXPosition - 0.4;
            
            if ((penLeftPos < coinRightPos)
                && (penRightPos >= coinLeftPos) || (penRightPos >= coinLeftPos)
                && (penLeftPos < coinRightPos)) {
                
                coinCollided = true;
                
            }
            if (coinCollided) {
                return coinCollision;
            }
        }
        //collision with cars
        if (lanesArray[getCurrentLane()].type) {
            
            if (!lanesArray[getCurrentLane()].isTruck) {
                //is car
                carRightPos = lanesArray[getCurrentLane()].getLaneCarXPosition()
                + (98.966 * 0.008) + 0.9;
                carLeftPos = lanesArray[getCurrentLane()].getLaneCarXPosition()
                - (98.966 * 0.008) - 0.75;
            } else { //is truck
                carRightPos = lanesArray[getCurrentLane()].getLaneCarXPosition()
                + (308.692 * 0.008) + 0.15;
                carLeftPos = lanesArray[getCurrentLane()].getLaneCarXPosition()
                - (308.692 * 0.008) - 0.4;
            }
            //collision with car's behind
            
            if ((penLeftPos < carRightPos) && (penRightPos >= carLeftPos) && this->penY < this->constantPenY - 0.2 + 0.4) {
                carCollided = true;
                
            }
            //collision with car's front
            if ((penRightPos >= carLeftPos) && (penLeftPos < carRightPos) && this->penY < this->constantPenY - 0.2 + 0.4) {
                carCollided = true;
            }
        }
        if (carCollided) {
            return carCollision;
        }
        
        return noCollision;
    }
    
    void setPenPosition(float penX, float penY, float penZ) {
        this->penX = penX;
        this->penY = penY;
        this->penZ = penZ;
        this->targetZ = penZ;
        this->initalZ = this->penZ;
    }
    void setPenX(float penX) {
        this->penX = penX;
    }
    void setPenY(float penY) {
        this->penY = penY;
    }
    void setPenZ(float penZ) {
        this->penZ = penZ;
    }
    float getPenX() {
        return this->penX;
    }
    float getPenY() {
        return this->penY;
    }
    float getPenZ() {
        return this->penZ;
    }
    PenModel* getPenguinMode() {
        return this->penguinModel;
    }
    int getCurrentLane() {
        return this->currentLaneIndex;
    }
    void setCurrentLaneIndex(int currentLane){
        this->currentLaneIndex = currentLane;
    }
    float getPenguinConstantY(){
        return constantPenY;
    }
private:
    const float penguinSpeed = 0.04f;
    const float constantPenY = 1.13f;
    Shader shader;
    Camera& camera;
    PenModel* penguinModel;
    GLfloat penX, penY, penZ, targetZ, initalZ;
    GLboolean isMoving = false;
    bool movingForwad = false, movingRight = false, movingLeft = false,
    movingBackward = false;
    int currentLaneIndex = 2;
    int adjacentLaneIndex;
    bool isMovedToAdjacentLane = false;
    
};
#endif
