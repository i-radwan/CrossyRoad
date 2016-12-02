#include "../Utilities/shaders.h"
#include <glm/glm.hpp>
#include <string>
using namespace std;
class Penguin{
public:
    Penguin(Shader shader, GLchar* modelLink):shader(shader){
        penguinModel = new Model(modelLink);
    }
    void draw(glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio, float near, float far){
        shader.Use();
        
        glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near, far);
        glm::mat4 objmodel;
        objmodel = glm::translate(objmodel, glm::vec3( this->penX, this->penY, this->penZ));
        objmodel = glm::rotate(objmodel, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        objmodel = glm::rotate(objmodel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        objmodel = glm::scale(objmodel, glm::vec3(0.12f, 0.12f, 0.12f));
        GLint objmodelLoc = glGetUniformLocation(shader.Program, "model");
        GLint objviewLoc = glGetUniformLocation(shader.Program, "view");
        GLint objprojectionLoc = glGetUniformLocation(shader.Program, "projection");
        glUniformMatrix4fv(objviewLoc, 1, GL_FALSE, glm::value_ptr(cameraViewMat));
        glUniformMatrix4fv(objprojectionLoc, 1, GL_FALSE, glm::value_ptr(objprojection));
        glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
        penguinModel->Draw(shader);
    }
    void setPenPosition(float penX, float penY, float penZ){
        this->penX = penX;
        this->penY = penY;
        this->penZ = penZ;
    }
    void setPenX(float penX){
        this->penX = penX;
    }
    void setPenY(float penY){
        this->penY = penY;
    }
    void setPenZ(float penZ){
        this->penZ = penZ;
    }
    float getPenX(){return this->penX;}
    float getPenY(){return this->penY;}
    float getPenZ(){return this->penZ;}
private:
    Shader shader;
    Model* penguinModel;
    float penX, penY, penZ;
    
};
