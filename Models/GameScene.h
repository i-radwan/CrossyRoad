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
    void draw(glm::mat4 lightSpaceMatrix, GLuint &depthMap, Camera &camera, glm::vec3 lightPos, GLboolean shadows, glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio, float near, float far, GLuint cubeVAO, GLuint cubeSafeLaneVAO, vector<lane> &lanesArray,float &newStart, Car& car, Car& truck, Coin& coin, Tree &tree){
        for(int i  = 0 ; i < lanesArray.size(); i++){
            //Draw Trees
            if(lanesArray[i].type == 0)
                tree.draw(lightSpaceMatrix, depthMap, camera, lightPos, shadows, cameraViewMat, cameraZoom, hwRatio, near, far, lanesArray[i].treeXpos, 0.432f, lanesArray[i].startPos );
        }
        //Draw lanes
        float zpos = newStart;
        for(GLuint i = 0; i < lanesArray.size(); i++)
        {
            if(lanesArray[i].type == 1){
                if(!lanesArray[i].isTruck){
                    car.draw(lightSpaceMatrix, depthMap,camera, lightPos, shadows, cameraViewMat, cameraZoom, hwRatio, near, far, lanesArray[i].getLaneCarXPosition(), 0.865, zpos, true);
                }
                else{
                    truck.draw(lightSpaceMatrix, depthMap,camera, lightPos, shadows,cameraViewMat, cameraZoom, hwRatio, near, far, lanesArray[i].getLaneCarXPosition(), 0.30, zpos, false);
                }
                lanesArray[i].moveCar();
            }
            
            // Draw the coin
            GLint randNum = (10 + (rand() % (int)(2000 - 10 + 1)));
            GLboolean addCoin = (randNum % 3 == 0);
            if(zpos < -4 && addCoin && !lanesArray[i].hasCoin && !lanesArray[i].drawnBefore){
                // ToDo Check if this x doesn't contain tree
                if(!lanesArray[i].isCoinConsumed){
                    GLfloat coinX = -6 + (rand() % (int)(9 - -6 + 1));
                    lanesArray[i].coinXPosition = coinX;
                    lanesArray[i].hasCoin = true;
                    coin.draw(lightSpaceMatrix, depthMap,camera, lightPos, shadows,cameraViewMat, cameraZoom, hwRatio, near, far, coinX, 0, zpos, lanesArray[i].coinRotation);
                    lanesArray[i].coinRotation++;
                    if(lanesArray[i].coinRotation > 360){
                        lanesArray[i].coinRotation -= 360;
                    }
                }
            }else if(lanesArray[i].hasCoin && !lanesArray[i].isCoinConsumed){
                coin.draw(lightSpaceMatrix, depthMap,camera, lightPos, shadows,cameraViewMat, cameraZoom, hwRatio, near, far, lanesArray[i].coinXPosition, 0, zpos, lanesArray[i].coinRotation);
                lanesArray[i].coinRotation++;
                if(lanesArray[i].coinRotation > 360){
                    lanesArray[i].coinRotation -= 360;
                }
            }
            
            shader.Use();
            glUniform3fv(glGetUniformLocation(shader.Program, "lightPos"), 1, &lightPos[0]);
            glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, &camera.Position[0]);
            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
            // Enable/Disable shadows by pressing 'SPACE'
            glUniform1i(glGetUniformLocation(shader.Program, "shadows"), shadows);
            
            
            glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near, far);
            
            GLint modelLoc = glGetUniformLocation(shader.Program, "model");
            GLint viewLoc = glGetUniformLocation(shader.Program, "view");
            GLint projectionLoc = glGetUniformLocation(shader.Program, "projection");
            GLint isSafeLaneLoc = glGetUniformLocation(shader.Program, "isSafeLane");
            GLint isSafeLaneAfterSafeLane = glGetUniformLocation(shader.Program, "isSafeLaneAfterSafeLane");
            GLint isNormalLaneBeforeNormalLane = glGetUniformLocation(shader.Program, "isNormalLaneBeforeNormalLane");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cameraViewMat));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(objprojection));
            glBindVertexArray(((lanesArray[i].type == 0) ? cubeVAO : cubeSafeLaneVAO));
            
            
            //BindDepthMap
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            
            
            glm::mat4 model;
            model = glm::translate(model, glm::vec3( 0.0f, 0.0f, zpos));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            if(lanesArray[i].type == 1){
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 2.0f));
            }
            if(lanesArray[i].type == 0){
                glUniform1f(isSafeLaneLoc, 1.0f);
                
            }
            if(i > 0 && lanesArray[i].type == 0 && lanesArray[i-1].type == 0){
                glUniform1f(isSafeLaneAfterSafeLane, 1.0f);
            }
            lanesArray[i].zpos = zpos;
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
            if(!lanesArray[i].drawnBefore)
                lanesArray[i].startPos =zpos,lanesArray[i].drawnBefore = 1;
            
            // Z operations
            if((i < lanesArray.size()-1 && lanesArray[i].type == 1 && lanesArray[i+1].type == 0) || (i < lanesArray.size()-1 && lanesArray[i].type == 0 && lanesArray[i+1].type == 1)){
                zpos -=1.5;
            }
            else{
                zpos -= 1;
            }
            
            // Draw lane separator
            if(i < lanesArray.size()-1 && lanesArray[i].type == 1 && lanesArray[i+1].type == 1){
                zpos -= 0.05;
                glm::mat4 model;
                model = glm::translate(model, glm::vec3( 0.0f, 0.0f, zpos));
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.1f));
                glUniform1f(isNormalLaneBeforeNormalLane, 1.0f);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                zpos -= 1.05;
            }
            // Reset uniforms
            glUniform1f(isSafeLaneLoc, 0.0f);
            glUniform1f(isSafeLaneAfterSafeLane, 0.0f);
            glUniform1f(isNormalLaneBeforeNormalLane, 0.0f);
        }
        glBindVertexArray(0);
        
    }
    
    
    void Render(Shader depthShader, vector<lane> &lanesArray,float &newStart, GLuint cubeVAO, GLuint cubeSafeLaneVAO, Car& car, Car& truck, Coin& coin, Tree &tree){
        //Render lanes
        float zpos = newStart;
        for(GLuint i = 0; i < lanesArray.size(); i++)
        {
            
            if(lanesArray[i].type == 1){
                if(!lanesArray[i].isTruck){
                    // RENDER CAR Car Render
                    //car.render();
                    car.Render(depthShader,lanesArray[i].getLaneCarXPosition(), 1.3, zpos, true);
                }
                else{
                    // RENDER TRUCK
                    truck.Render(depthShader,lanesArray[i].getLaneCarXPosition(), 1.3, zpos, false);
                }
                lanesArray[i].moveCar();
            }
            // RENDER the coin
            GLint randNum = (10 + (rand() % (int)(2000 - 10 + 1)));
            GLboolean addCoin = (randNum % 3 == 0);
            if(((zpos < -4 && addCoin && !lanesArray[i].hasCoin && !lanesArray[i].drawnBefore) || lanesArray[i].hasCoin) &&( !lanesArray[i].isCoinConsumed)){
                //RENDER COIN
                coin.Render(depthShader, lanesArray[i].coinXPosition, 0, zpos, lanesArray[i].coinRotation);
                lanesArray[i].coinRotation++;
                if(lanesArray[i].coinRotation > 360){
                    lanesArray[i].coinRotation -= 360;
                }
            }
            GLint modelLoc = glGetUniformLocation(depthShader.Program, "model");
            glBindVertexArray(((lanesArray[i].type == 0) ? cubeVAO : cubeSafeLaneVAO));
            
            glm::mat4 model;
            model = glm::translate(model, glm::vec3( 0.0f, 0.0f, zpos));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            if(lanesArray[i].type == 1){
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 2.0f));
            }
            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            //glDrawArrays(GL_TRIANGLES, 0, 36);
            
            if(!lanesArray[i].drawnBefore)
                lanesArray[i].startPos =zpos,lanesArray[i].drawnBefore = 1;
            
            // Z operations
            if((i < lanesArray.size()-1 && lanesArray[i].type == 1 && lanesArray[i+1].type == 0) || (i < lanesArray.size()-1 && lanesArray[i].type == 0 && lanesArray[i+1].type == 1)){
                zpos -=1.5;
            }
            else{
                zpos -= 1;
            }
            //Render Trees
            if(lanesArray[i].type == 0)
                tree.Render(depthShader,lanesArray[i].treeXpos, 0.432f, lanesArray[i].startPos );
            
            // Draw lane separator
            if(i < lanesArray.size()-1 && lanesArray[i].type == 1 && lanesArray[i+1].type == 1){
                zpos -= 0.05;
                glm::mat4 model;
                model = glm::translate(model, glm::vec3( 0.0f, 0.0f, zpos));
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.1f));
                
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                //  glDrawArrays(GL_TRIANGLES, 0, 36);
                zpos -= 1.05;
            }
            
        }
        glBindVertexArray(0);
    }
private:
    Shader shader;
};
