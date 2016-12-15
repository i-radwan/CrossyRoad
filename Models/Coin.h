using namespace std;
class Coin{
public:
    Coin(Shader shader, GLchar* modelLink):shader(shader){
        coinModel = new Model(modelLink);
        shader.Use();
        glUniform1i(glGetUniformLocation(shader.Program, "diffuseTexture"), 0);
        glUniform1i(glGetUniformLocation(shader.Program, "shadowMap"), 1);
        
    }
    void draw(glm::mat4 lightSpaceMatrix, GLuint &depthMap, Camera &camera, glm::vec3 lightPos, GLboolean shadows, glm::mat4 cameraViewMat, GLfloat cameraZoom, float hwRatio, float near, float far, float coinX, float coinY, float coinZ, float rotationAngle){
        shader.Use();

        // Set light uniforms
        glUniform3fv(glGetUniformLocation(shader.Program, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, &camera.Position[0]);
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        
        // Enable/Disable shadows by pressing 'SPACE'
        glUniform1i(glGetUniformLocation(shader.Program, "shadows"), shadows);
        
        glm::mat4 objprojection = glm::perspective(cameraZoom, hwRatio, near, far);
        glm::mat4 objmodel;
        objmodel = glm::translate(objmodel, glm::vec3( coinX, 1.6, coinZ)); // ToDo use coinY
        objmodel = glm::rotate(objmodel, glm::radians(-rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
//        objmodel = glm::rotate(objmodel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        objmodel = glm::rotate(objmodel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        objmodel = glm::scale(objmodel, glm::vec3(0.1f, 0.1f, 0.1f));
        GLint objmodelLoc = glGetUniformLocation(shader.Program, "model");
        GLint objviewLoc = glGetUniformLocation(shader.Program, "view");
        GLint objprojectionLoc = glGetUniformLocation(shader.Program, "projection");
        glUniformMatrix4fv(objviewLoc, 1, GL_FALSE, glm::value_ptr(cameraViewMat));
        glUniformMatrix4fv(objprojectionLoc, 1, GL_FALSE, glm::value_ptr(objprojection));
        glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
        coinModel->Draw(shader);
    }
    void Render(Shader &depthShader, float coinX, float coinY, float coinZ, float rotationAngle){
        glm::mat4 objmodel;
        objmodel = glm::translate(objmodel, glm::vec3( coinX, 1.6, coinZ)); // ToDo use coinY
        objmodel = glm::rotate(objmodel, glm::radians(-rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
//        objmodel = glm::rotate(objmodel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        objmodel = glm::rotate(objmodel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //objmodel = glm::scale(objmodel, glm::vec3(0.4f, 0.4f, 0.4f));
        objmodel = glm::scale(objmodel, glm::vec3(0.1f, 0.1f, 0.1f));
        GLint objmodelLoc = glGetUniformLocation(depthShader.Program, "model");
        glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
        coinModel->Render();
    }
private:
    Shader shader;
    Model* coinModel;
};
