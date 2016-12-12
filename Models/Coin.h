using namespace std;
class Coin{
public:
    Coin(Shader shader, GLchar* modelLink):shader(shader){
        coinModel = new Model(modelLink);
    }
    void draw(glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio, float near, float far, float coinX, float coinY, float coinZ, float rotationAngle){
        shader.Use();
        glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
        GLint lightColorLoc = glGetUniformLocation(shader.Program, "lightColor");
        glUniform3f(lightColorLoc, lightCol.x, lightCol.y, lightCol.z);
        
        glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near, far);
        glm::mat4 objmodel;
        objmodel = glm::translate(objmodel, glm::vec3( coinX, 1.6, coinZ)); // ToDo use coinY
        objmodel = glm::rotate(objmodel, glm::radians(-rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        objmodel = glm::rotate(objmodel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        objmodel = glm::scale(objmodel, glm::vec3(0.4f, 0.4f, 0.4f));
        GLint objmodelLoc = glGetUniformLocation(shader.Program, "model");
        GLint objviewLoc = glGetUniformLocation(shader.Program, "view");
        GLint objprojectionLoc = glGetUniformLocation(shader.Program, "projection");
        glUniformMatrix4fv(objviewLoc, 1, GL_FALSE, glm::value_ptr(cameraViewMat));
        glUniformMatrix4fv(objprojectionLoc, 1, GL_FALSE, glm::value_ptr(objprojection));
        glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
        coinModel->Draw(shader);
    }
private:
    Shader shader;
    Model* coinModel;
};
