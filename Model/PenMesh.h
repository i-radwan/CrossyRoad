#include <vector>
#include <string>
#include "../Utilities/shaders.h"

using namespace std;
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};
struct Texture {
    GLuint id;
    string type;
    aiString path;
};
struct ObjColor {
    glm::vec4 diffColor;
    glm::vec4 specColor;
    glm::vec4 ambiColor;
    glm::vec4 emiColor;
};
struct animationData {
    GLfloat time;
    struct Position{
        GLfloat x, y, z;
    }position;
    struct Rotation{
        GLfloat x, y, z, w;
    }rotation;
    struct Scaling{
        GLfloat x, y, z;
    }scaling;
};
class PenMesh {
public:
    /* Mesh Data */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    ObjColor colors;
    string name;
    /* Functions */
    PenMesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, ObjColor colors, string name)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->colors= colors;
        this->name = name;
        this->setupMesh();
    }
    void Draw(Shader shader, vector<pair< string, vector<animationData> > > anim, GLuint frameCount,  glm::mat4 objmodel, bool moving)
    {
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        for(GLuint i = 0; i < this->textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
            // Retrieve texture number (the N in diffuse_textureN)
            stringstream ss;
            string number;
            string name = this->textures[i].type;
            if(name == "texture_diffuse")
                ss << diffuseNr++; // Transfer GLuint to stream
            else if(name == "texture_specular")
                ss << specularNr++; // Transfer GLuint to stream
            number = ss.str();
            glUniform1f(glGetUniformLocation(shader.Program, ("material." +
                                                              name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
        }
        glUniform4f(glGetUniformLocation(shader.Program, "diffuseColor"), this->colors.diffColor.r,this->colors.diffColor.g,this->colors.diffColor.b,this->colors.diffColor.a);
        
        glUniform4f(glGetUniformLocation(shader.Program, "ambientColor"), this->colors.ambiColor.r,this->colors.ambiColor.g,this->colors.ambiColor.b,this->colors.ambiColor.a);
        
        glUniform4f(glGetUniformLocation(shader.Program, "specularColor"), this->colors.specColor.r,this->colors.specColor.g,this->colors.specColor.b,this->colors.specColor.a);
        
        glUniform4f(glGetUniformLocation(shader.Program, "emiColor"), this->colors.emiColor.r,this->colors.emiColor.g,this->colors.emiColor.b,this->colors.emiColor.a);
        
        // Before drawing the mesh get from anim vector the animation value corrsponding to the node name and current framCount and apply these changes to the objmodel mat4 and then set the uniform into the shader and then draw
        if(moving)
            for (int i = 0; i < anim.size(); i++) {
                if(anim[i].first == this->name){
                    animationData animationdata =  anim[i].second[frameCount];
                    //                objmodel = glm::rotate(objmodel, (animationdata.rotation.x), glm::vec3(1.0,0,0));
                    //                objmodel = glm::rotate(objmodel, (animationdata.rotation.y), glm::vec3(0,1.0,0));
                    objmodel = glm::rotate(objmodel, (animationdata.rotation.z * 3), glm::vec3(0,0,1.0));
                    GLint objmodelLoc = glGetUniformLocation(shader.Program, "model");
                    glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
                    break;
                }
            }
        glActiveTexture(GL_TEXTURE0);
        // Draw mesh
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    void Render(Shader &depthShader, vector<pair< string, vector<animationData> > > anim, GLuint frameCount,  glm::mat4 objmodel, bool moving){
        // Before drawing the mesh get from anim vector the animation value corrsponding to the node name and current framCount and apply these changes to the objmodel mat4 and then set the uniform into the shader and then draw
        if(moving)
            for (int i = 0; i < anim.size(); i++) {
                if(anim[i].first == this->name){
                    animationData animationdata =  anim[i].second[frameCount];
                    //                objmodel = glm::rotate(objmodel, (animationdata.rotation.x), glm::vec3(1.0,0,0));
                    //                objmodel = glm::rotate(objmodel, (animationdata.rotation.y), glm::vec3(0,1.0,0));
                    objmodel = glm::rotate(objmodel, (animationdata.rotation.z * 3), glm::vec3(0,0,1.0));
                    GLint objmodelLoc = glGetUniformLocation(depthShader.Program, "model");
                    glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
                    break;
                }
            }
        // Draw mesh
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
    }
private:
    /* Render data */
    GLuint VAO, VBO, EBO;
    /* Functions    */
    void setupMesh()
    {
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);
        
        glBindVertexArray(this->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
                     &this->vertices[0], GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof( GLuint), &this->indices[0], GL_STATIC_DRAW);
        
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              // Vertex Normals
                              (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, Normal));
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, TexCoords));
        glBindVertexArray(0);
        
    }
};

