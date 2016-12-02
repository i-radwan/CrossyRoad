#include "../Utilities/shaders.h"
#include <glm/glm.hpp>
#include <string>
using namespace std;
class GameScene{
public:
    GameScene(Shader shader):shader(shader){
    }
    void draw(glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio, float near, float far, GLuint cubeAVO, vector<int> lanes){
        shader.Use();
        
        glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near, far);
        
//        glm::mat4 objmodel;
//        objmodel = glm::translate(objmodel, glm::vec3( this->penX, this->penY, this->penZ));
//        objmodel = glm::rotate(objmodel, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//        objmodel = glm::rotate(objmodel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//        objmodel = glm::scale(objmodel, glm::vec3(0.15f, 0.15f, 0.15f));
//        
        
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
        float zpos = 0;
        for(GLuint i = 0; i < lanes.size(); i++)
        {
            glm::mat4 model;
            zpos -= 1;
            model = glm::translate(model, glm::vec3( 0.0f, 0.0f, zpos));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            
            if(lanes[i] == 0){
                glUniform1f(isSafeLaneLoc, 1.0f);
            }
            if(i > 0 && lanes[i] == 0 && lanes[i-1] == 0){
                glUniform1f(isSafeLaneAfterSafeLane, 1.0f);
            }
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
            // Draw lane separator
            if(i < lanes.size()-1 && lanes[i] == 1 && lanes[i+1] == 1){
                zpos -= 1.1;
                glm::mat4 model;
                model = glm::translate(model, glm::vec3( 0.0f, 0.0f, zpos));
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.08f));
                glUniform1f(isNormalLaneBeforeNormalLane, 1.0f);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                zpos -= 0.1;
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