#include "../Utilities/shaders.h"
#include <glm/glm.hpp>
#include <string>
using namespace std;
class GameScene{
public:
    GameScene(Shader shader):shader(shader){
    }
    void draw(glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio, float near, float far, GLuint cubeAVO, vector<lane> &lanesArray,float &newStart){
        shader.Use();
        glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
        GLint lightColorLoc = glGetUniformLocation(shader.Program, "lightColor");
        glUniform3f(lightColorLoc, lightCol.x, lightCol.y, lightCol.z);
        
        glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near, far);
        
        GLint modelLoc = glGetUniformLocation(shader.Program, "model");
        GLint viewLoc = glGetUniformLocation(shader.Program, "view");
        GLint projectionLoc = glGetUniformLocation(shader.Program, "projection");
        GLint isSafeLaneLoc = glGetUniformLocation(shader.Program, "isSafeLane");
        GLint isSafeLaneAfterSafeLane = glGetUniformLocation(shader.Program, "isSafeLaneAfterSafeLane");
        GLint isNormalLaneBeforeNormalLane = glGetUniformLocation(shader.Program, "isNormalLaneBeforeNormalLane");
        
        
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cameraViewMat));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(objprojection));
        
        glBindVertexArray(cubeAVO);
        
        //Draw lanes
        float zpos = newStart;
        for(GLuint i = 0; i < lanesArray.size(); i++)
        {
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
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
            
            // Z operations
            if((i < lanesArray.size()-1 && lanesArray[i].type == 1 && lanesArray[i+1].type == 0) || (i < lanesArray.size()-1 && lanesArray[i].type == 0 && lanesArray[i+1].type == 1)){
                zpos -=1.5;
            }
            else{
                zpos -= 1;
            }
            if(!lanesArray[i].drawnBefore)
                lanesArray[i].startPos =zpos,lanesArray[i].drawnBefore = 1;
            
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
private:
    Shader shader;
};
