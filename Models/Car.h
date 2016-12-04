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
    void draw(glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio, float near, float far, float carX, float carY, float carZ, float penguinZpos, float penguinXpos){
        shader.Use();
        
        // Use cooresponding shader when setting uniforms/drawing objects
        glm::vec3 lightPos = glm::vec3(penguinXpos, 4.0f, penguinZpos +3);
        glm::vec3 lightCol = glm::vec3(0.55f, 0.55f, 0.55f);
        GLint lightColorLoc = glGetUniformLocation(shader.Program, "lightColor");
        GLint lightPosLoc    = glGetUniformLocation(shader.Program, "lightPos");
        GLint factorLoc = glGetUniformLocation(shader.Program, "factor");
        glUniform3f(lightColorLoc, lightCol.x, lightCol.y, lightCol.z);
        glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
        float factor = 1;
        //float factor = (penguinZpos - carZ) / (penguinZpos);
//        factor = (factor < 0 ? -factor: factor);
//        factor = factor > penguinZpos-2 ? (1-factor)+0.5:factor ;
        glUniform1f(factorLoc, factor);
        
        glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near, far);
        glm::mat4 objmodel;
        objmodel = glm::translate(objmodel, glm::vec3( carX, carY, carZ - 0.25f));
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
};
