#ifndef GAME_SCENE
#define GAME_SCENE
#include "../Utilities/shaders.h"
#include "../Utilities/utils.h"
#include "../Models/Coin.h"
#include "../Models/Tree.h"
#include <glm/glm.hpp>
#include <string>
using namespace std;
class GameScene{
public:
    GameScene(Shader shader):shader(shader){
        shader.Use();
        glUniform1i(glGetUniformLocation(shader.Program, "shadowMap"), 0);
        
    }
    void draw(glm::mat4 lightSpaceMatrix, GLuint &depthMap, Camera &camera, glm::vec3 lightPos, GLboolean areShadowsEnabled, glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio, float near, float far, GLuint cubeVAO, GLuint cubeSafeLaneVAO, vector<Lane> &lanesArray, Car& car, Car& truck, Coin& coin, Tree &tree){
        //Draw lanes
        for(GLuint i = 0; i < lanesArray.size(); i++)
        {
            //Draw Trees
            if(lanesArray[i].type == LaneType::SAFE_LANE)
                tree.draw(lightSpaceMatrix, depthMap, camera, lightPos, areShadowsEnabled, cameraViewMat, cameraZoom, hwRatio, near, far, lanesArray[i].treeXpos, Constants::TREE_Y, lanesArray[i].laneZPos);
            if(lanesArray[i].type == LaneType::NORMAL_LANE){
                if(!lanesArray[i].isTruck){
                    car.draw(lightSpaceMatrix, depthMap,camera, lightPos, areShadowsEnabled, cameraViewMat, cameraZoom, hwRatio, near, far, lanesArray[i].getLaneCarXPosition(), Constants::CAR_Y, lanesArray[i].laneZPos, true);
                }
                else{
                    truck.draw(lightSpaceMatrix, depthMap,camera, lightPos, areShadowsEnabled,cameraViewMat, cameraZoom, hwRatio, near, far, lanesArray[i].getLaneCarXPosition(), Constants::TRUCK_Y, lanesArray[i].laneZPos, false);
                }
            }
            
            // Draw the coin
            if(lanesArray[i].hasCoin && !lanesArray[i].isCoinConsumed){
                coin.draw(lightSpaceMatrix, depthMap,camera, lightPos, areShadowsEnabled,cameraViewMat, cameraZoom, hwRatio, near, far, lanesArray[i].coinXPosition, Constants::COIN_Y, lanesArray[i].laneZPos, lanesArray[i].coinRotation);
            }
            shader.Use();
            glUniform3fv(glGetUniformLocation(shader.Program, "lightPos"), 1, &lightPos[0]);
            glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, &camera.Position[0]);
            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
            glUniform1i(glGetUniformLocation(shader.Program, "shadows"), areShadowsEnabled);
            glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near, far);
            
            GLint modelLoc = glGetUniformLocation(shader.Program, "model");
            GLint viewLoc = glGetUniformLocation(shader.Program, "view");
            GLint projectionLoc = glGetUniformLocation(shader.Program, "projection");
            GLint isSafeLaneLoc = glGetUniformLocation(shader.Program, "isSafeLane");
            GLint isSafeLaneAfterSafeLane = glGetUniformLocation(shader.Program, "isSafeLaneAfterSafeLane");
            GLint isNormalLaneBeforeNormalLane = glGetUniformLocation(shader.Program, "isNormalLaneBeforeNormalLane");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cameraViewMat));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(objprojection));
            glBindVertexArray(((lanesArray[i].type == LaneType::NORMAL_LANE) ? cubeVAO : cubeSafeLaneVAO));
            
            
            //BindDepthMap
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            
            
            glm::mat4 model;
            model = glm::translate(model, glm::vec3( 0.0f, 0.0f, lanesArray[i].laneZPos));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            if(lanesArray[i].type == LaneType::NORMAL_LANE){
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 2.0f));
            }
            if(lanesArray[i].type == LaneType::SAFE_LANE){
                glUniform1f(isSafeLaneLoc, 1.0f);
                
            }
            if(i > 0 && lanesArray[i].type == LaneType::SAFE_LANE && lanesArray[i-1].type == LaneType::SAFE_LANE){
                glUniform1f(isSafeLaneAfterSafeLane, 1.0f);
            }
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
            if(!lanesArray[i].drawnBefore)
                lanesArray[i].drawnBefore = 1;
            
            // Draw Lane separator
            if(i < lanesArray.size()-1 && lanesArray[i].type == LaneType::NORMAL_LANE && lanesArray[i+1].type == LaneType::NORMAL_LANE){
                glm::mat4 model;
                model = glm::translate(model, glm::vec3( 0.0f, 0.0f, lanesArray[i+1].laneZPos + 1.05));
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.1f));
                glUniform1f(isNormalLaneBeforeNormalLane, 1.0f);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            // Reset uniforms
            glUniform1f(isSafeLaneLoc, 0.0f);
            glUniform1f(isSafeLaneAfterSafeLane, 0.0f);
            glUniform1f(isNormalLaneBeforeNormalLane, 0.0f);
        }
        glBindVertexArray(0);
        
    }
    
    
    void render(Shader depthShader, vector<Lane> &lanesArray, GLuint cubeVAO, GLuint cubeSafeLaneVAO, Car& car, Car& truck, Coin& coin, Tree &tree){
        //Render lanes
        for(GLuint i = 0; i < lanesArray.size(); i++)
        {
            
            if(lanesArray[i].type == LaneType::NORMAL_LANE){
                if(!lanesArray[i].isTruck){
                    // RENDER CAR
                    car.render(depthShader,lanesArray[i].getLaneCarXPosition(), Constants::CAR_Y, lanesArray[i].laneZPos, true);
                }
                else{
                    // RENDER TRUCK
                    truck.render(depthShader,lanesArray[i].getLaneCarXPosition(), Constants::TRUCK_Y, lanesArray[i].laneZPos, false);
                }
                lanesArray[i].moveCar();
            }
            // RENDER the coin
            if(lanesArray[i].hasCoin && !lanesArray[i].isCoinConsumed){
                coin.render(depthShader, lanesArray[i].coinXPosition, Constants::COIN_Y, lanesArray[i].laneZPos, lanesArray[i].coinRotation);
                lanesArray[i].rotateCoin();
            }
            
            
            GLint modelLoc = glGetUniformLocation(depthShader.Program, "model");
            glBindVertexArray(((lanesArray[i].type == LaneType::SAFE_LANE) ? cubeVAO : cubeSafeLaneVAO));
            
            glm::mat4 model;
            model = glm::translate(model, glm::vec3( 0.0f, 0.0f, lanesArray[i].laneZPos));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            if(lanesArray[i].type == LaneType::NORMAL_LANE){
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 2.0f));
            }
            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            
            if(!lanesArray[i].drawnBefore)
                lanesArray[i].drawnBefore = 1;
            
            //Render Trees
            if(lanesArray[i].type == LaneType::SAFE_LANE)
                tree.render(depthShader, lanesArray[i].treeXpos, Constants::TREE_Y, lanesArray[i].laneZPos );
            
            // Draw Lane separator
            if(i < lanesArray.size()-1 && lanesArray[i].type == LaneType::NORMAL_LANE && lanesArray[i+1].type == 1){
                glm::mat4 model;
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, lanesArray[i+1].laneZPos + 1.05));
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.1f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            }
            
        }
        glBindVertexArray(0);
    }
private:
    Shader shader;
};
#endif
