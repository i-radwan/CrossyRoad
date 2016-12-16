#include "../Utilities/shaders.h"
#include <glm/glm.hpp>
#include <string>
#include "../Model/Model.h"
using namespace std;
class Car{
public:
    Car(Shader shader, GLchar* modelLink):shader(shader){
        carModel = new Model(modelLink);
        shader.Use();
        glUniform1i(glGetUniformLocation(shader.Program, "diffuseTexture"), 0);
        glUniform1i(glGetUniformLocation(shader.Program, "shadowMap"), 1);
    }
    void draw(glm::mat4 lightSpaceMatrix, GLuint &depthMap, Camera &camera, glm::vec3 lightPos, GLboolean shadows,glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio, float near, float far, float carX, float carY, float carZ, bool carRotation){
        shader.Use();
        // Set light uniforms
        glUniform3fv(glGetUniformLocation(shader.Program, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, &camera.Position[0]);
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        
        // Enable/Disable shadows by pressing 'SPACE'
        glUniform1i(glGetUniformLocation(shader.Program, "shadows"), shadows);

        
        glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near, far);
        glm::mat4 objmodel;
        objmodel = glm::translate(objmodel, glm::vec3( carX, carY, carZ));
        if(carRotation){ // Means small car
            objmodel = glm::rotate(objmodel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            objmodel = glm::scale(objmodel, glm::vec3(0.008f, 0.008f, 0.008f));
        } else{ // Means truck
            objmodel = glm::rotate(objmodel, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            objmodel = glm::rotate(objmodel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            objmodel = glm::scale(objmodel, glm::vec3(0.008f, 0.008f, 0.008f));
        }
        GLint objmodelLoc = glGetUniformLocation(shader.Program, "model");
        GLint objviewLoc = glGetUniformLocation(shader.Program, "view");
        GLint objprojectionLoc = glGetUniformLocation(shader.Program, "projection");
        glUniformMatrix4fv(objviewLoc, 1, GL_FALSE, glm::value_ptr(cameraViewMat));
        glUniformMatrix4fv(objprojectionLoc, 1, GL_FALSE, glm::value_ptr(objprojection));
        glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
        carModel->Draw(shader, depthMap);
    }
    void Render(Shader &depthShader, float carX, float carY, float carZ, bool carRotation){
        glm::mat4 objmodel;
        GLint objmodelLoc = glGetUniformLocation(depthShader.Program, "model");
        if(carRotation){ // Means small car
            objmodel = glm::translate(objmodel, glm::vec3( carX, carY, carZ));
            objmodel = glm::rotate(objmodel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            objmodel = glm::scale(objmodel, glm::vec3(0.008f, 0.008f, 0.008f));
        } else{ // Means truck
            objmodel = glm::translate(objmodel, glm::vec3( carX, 0.5, carZ));
            objmodel = glm::rotate(objmodel, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            objmodel = glm::rotate(objmodel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            objmodel = glm::scale(objmodel, glm::vec3(0.008f, 0.008f, 0.008f));
        }
        glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
        carModel->Render();
    }
private:
    Shader shader;
    Model* carModel;
    GLint speed;
    GLint xPos;
    
};
