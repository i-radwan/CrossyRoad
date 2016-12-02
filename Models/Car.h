#include "../Utilities/shaders.h"
#include <glm/glm.hpp>
#include <string>
using namespace std;
class Car{
public:
    Car(Shader shader, GLchar* modelLink):shader(shader){
        carModel = new Model(modelLink);
    }
    void draw(glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio, float near, float far, float penX, float penY, float penZ){
        shader.Use();
        glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near, far);
        glm::mat4 objmodel;
        objmodel = glm::translate(objmodel, glm::vec3( penX, penY, penZ));
        objmodel = glm::rotate(objmodel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        objmodel = glm::scale(objmodel, glm::vec3(0.006f, 0.006f, 0.006f)); // ToDo size up
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
