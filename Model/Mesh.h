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

class Mesh {
public:
    /* Mesh Data */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    ObjColor colors;
    /* Functions */
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, ObjColor colors)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->colors= colors;
        this->setupMesh();
    }
    void Draw(Shader shader)
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
        
        glActiveTexture(GL_TEXTURE0);
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

