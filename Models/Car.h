#include "../Utilities/shaders.h"
#include <glm/glm.hpp>
#include <string>
#include "../Model/Model.h"
using namespace std;
class Car{
public:
    Car(Shader shader, GLchar* modelLink):shader(shader){
        carModel = new Model(modelLink);
    }
    void draw(glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio, float near, float far, float carX, float carY, float carZ){
        shader.Use();
        glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
        GLint lightColorLoc = glGetUniformLocation(shader.Program, "lightColor");
        glUniform3f(lightColorLoc, lightCol.x, lightCol.y, lightCol.z);
        
        glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near, far);
        glm::mat4 objmodel;
        objmodel = glm::translate(objmodel, glm::vec3( carX, carY, carZ));
        objmodel = glm::rotate(objmodel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        objmodel = glm::scale(objmodel, glm::vec3(0.008f, 0.008f, 0.008f));
        GLint objmodelLoc = glGetUniformLocation(shader.Program, "model");
        GLint objviewLoc = glGetUniformLocation(shader.Program, "view");
        GLint objprojectionLoc = glGetUniformLocation(shader.Program, "projection");
        glUniformMatrix4fv(objviewLoc, 1, GL_FALSE, glm::value_ptr(cameraViewMat));
        glUniformMatrix4fv(objprojectionLoc, 1, GL_FALSE, glm::value_ptr(objprojection));
        glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
        carModel->Draw(shader);
    }
private:
    Shader shader;
    Model* carModel;
    GLint speed;
    GLint xPos;
    
};
