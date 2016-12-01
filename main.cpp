#include <iostream>
#include <GL/glew.h>
#include <cmath>
#include <GLFW/glfw3.h>
#include "shaders.h"
#include "camera.h"
#include <SOIL.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

bool keys[1024];
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 4.0f, 5.0f));
GLfloat lastX = 400, lastY = 300;
GLboolean firstMouse = true;


const GLint gameWidth = 750;
const GLint gameHeight = 800;

void do_movement();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


//Work for Model loading
GLint TextureFromFile(const char* path, string directory);
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

vector<Texture> textures_loaded;

class Mesh {
public:
    /* Mesh Data */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    /* Functions */
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
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


class Model
{
public:
    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model(GLchar* path)
    {
        this->loadModel(path);
    }
    
    // Draws the model, and thus all its meshes
    void Draw(Shader shader)
    {
        for(GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].Draw(shader);
    }
    
private:
    /*  Model Data  */
    vector<Mesh> meshes;
    string directory;
    
    /*  Functions   */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string path)
    {
        // Read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        // Check for errors
        if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // Retrieve the directory path of the filepath
        this->directory = path.substr(0, path.find_last_of('/'));
        // Process ASSIMP's root node recursively
        this->processNode(scene->mRootNode, scene);
    }
    
    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // Process each mesh located at the current node
        for(GLuint i = 0; i < node->mNumMeshes; i++)
        {
            // The node object only contains indices to index the actual objects in the scene.
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh, scene));
        }
        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(GLuint i = 0; i < node->mNumChildren; i++)
        {
            this->processNode(node->mChildren[i], scene);
        }
        
    }
    
    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // Data to fill
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        
        // Walk through each of the mesh's vertices
        for(GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // Normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // Texture Coordinates
            if(mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            vertices.push_back(vertex);
        }
        // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // Retrieve all indices of the face and store them in the indices vector
            for(GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // Process materials
        if(mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
            // Same applies to other texture as the following list summarizes:
            // Diffuse: texture_diffuseN
            // Specular: texture_specularN
            // Normal: texture_normalN
            
            // 1. Diffuse maps
            vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. Specular maps
            vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        
        // Return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }
    
    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                         string typeName)
    {
        vector<Texture> textures;
        for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            GLboolean skip = false;
            for(GLuint j = 0; j < textures_loaded.size(); j++)
            {
                if(textures_loaded[j].path == str)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                } }
            if(!skip)
            { // If texture hasn’t been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                textures_loaded.push_back(texture); // Add to loaded  textures
            }
        }
        return textures;
    }
};


int main(int argc, const char * argv[]) {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Create and configure window
    GLFWwindow* window = glfwCreateWindow(gameWidth, gameHeight, "CrossyRoad", NULL, NULL);
    
    if (window == nullptr)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glewExperimental = GL_TRUE;
    //    glViewport(0, 0, gameWidth, gameHeight);
    
    if (glewInit() != GLEW_OK)
    {
        cout << "Failed to initialize GLEW" << endl;
        return -1;
    }
    // Load shaders (normal shader and light source shader)
    Shader mainShader("shader.vs", "shader.frag");
    Shader objShader("objshader.vs", "objshader.frag");
    // Set vectices
    glEnable(GL_DEPTH_TEST);
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // Bind vertex buffer object and vertex array object and set buffer data from vertices object
    // Set the attributes in shaders
    
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    
    // Get yOffset uniform location (to draw 4 lanes)
    GLint yOffset = glGetUniformLocation(mainShader.Program, "yOffset");
    GLint safeLand = glGetUniformLocation(mainShader.Program, "safeLand");
    
    // ToDo: Algorithm to generate this array with the following conditions:
    // • lanesData[]: random sequence of 0's and 1's while maintaining the following:
    //              - No more than 2 safe lands next to each other
    //              - No more than 5 normal lanes next to each other
    //              - Start with a safe lane
    
    int lanesData[] = {0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0 ,1 ,1,  1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0}; // 0 safe lane, 1 normal lane
    std::vector<int> lanes (lanesData, lanesData + sizeof(lanesData) / sizeof(int) );
    
    //Model loading test
    Model mainCharModel("/Users/ibrahimradwan/Desktop/obj/duck_obj.obj");
    Model carModel("/Users/ibrahimradwan/Desktop/Small_car_obj/Small car.obj");
    
    float objzpos = -3, objxpos = 0;
    
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents(); // Check for keyboard or mouse events
        // Calculate frame time for smooth/quick movement on all devices
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if generation needed
        if (camera.Position.z < (int)(-1*lanes.size())){
            for (int i = 1; i < 52; i++) {
                lanes.push_back(lanesData[i]);
            }
        }
        //Check for arrows movement
        do_movement();
        
        // Clear screen with certain background color
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        // Clear depth and color buffers with each iteration so the new values can be applied without blocking
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Indicate to gl that we are going to use the original shaders for now
        
        // Main Character Object drawing
        objShader.Use();
        glm::mat4 objview = camera.GetViewMatrix();
        
        glm::mat4 objprojection = glm::perspective(glm::radians(camera.Zoom), (float) gameHeight/(float)gameWidth, 0.1f, 1000.0f);
        glm::mat4 objmodel;
        objmodel = glm::translate(objmodel, glm::vec3( objxpos, 0.0f, objzpos));
        objmodel = glm::rotate(objmodel, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        objmodel = glm::rotate(objmodel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        objmodel = glm::scale(objmodel, glm::vec3(0.02f, 0.02f, 0.02f));
        GLint objmodelLoc = glGetUniformLocation(objShader.Program, "model");
        GLint objviewLoc = glGetUniformLocation(objShader.Program, "view");
        GLint objprojectionLoc = glGetUniformLocation(objShader.Program, "projection");
        glUniformMatrix4fv(objviewLoc, 1, GL_FALSE, glm::value_ptr(objview));
        glUniformMatrix4fv(objprojectionLoc, 1, GL_FALSE, glm::value_ptr(objprojection));
        glUniformMatrix4fv(objmodelLoc, 1, GL_FALSE, glm::value_ptr(objmodel));
        
        mainCharModel.Draw(objShader);
        
        // Cars Objects drawing
        objShader.Use();
        glm::mat4 carview = camera.GetViewMatrix();
        glm::mat4 carprojection = glm::perspective(glm::radians(camera.Zoom), (float) gameHeight/(float)gameWidth, 0.1f, 1000.0f);
        //Draw cars
        GLint carviewLoc = glGetUniformLocation(objShader.Program, "view");
        GLint carprojectionLoc = glGetUniformLocation(objShader.Program, "projection");
        glUniformMatrix4fv(carviewLoc, 1, GL_FALSE, glm::value_ptr(carview));
        glUniformMatrix4fv(carprojectionLoc, 1, GL_FALSE, glm::value_ptr(carprojection));
        
        float carsZPos = 0;
        for(GLuint i = 0; i < lanes.size(); i++)
        {
            carsZPos -=1;
            if(lanes[i] == 1){
                glm::mat4 carmodel;
                //                carmodel = glm::translate(carmodel, glm::vec3(25.0f, 0.9f, carsZPos));
                carmodel = glm::translate(carmodel, glm::vec3(0.0f, 0.9f, carsZPos));
                carmodel = glm::rotate(carmodel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                carmodel = glm::scale(carmodel, glm::vec3(0.006f, 0.006f, 0.006f));
                GLint carmodelLoc = glGetUniformLocation(objShader.Program, "model");
                glUniformMatrix4fv(carmodelLoc, 1, GL_FALSE, glm::value_ptr(carmodel));
                
                carModel.Draw(objShader);
            }
            if(i < lanes.size()-1 && lanes[i] == 1 && lanes[i+1] == 1){
                carsZPos -= 1.2;
            }
            
        }
        
        
        if(keys[GLFW_KEY_UP]){
            objzpos -= 0.1f;
            //           camera.ProcessKeyboard(FORWARD, deltaTime);
        }
        if(keys[GLFW_KEY_DOWN]){
            objzpos += 0.1f;
            //            camera.ProcessKeyboard(BACKWARD, deltaTime);
        }
        if(keys[GLFW_KEY_LEFT])
            objxpos -= 0.1f;
        if(keys[GLFW_KEY_RIGHT])
            objxpos += 0.1f;
        
        
        
        // Set model, view, projection matrices
        
        mainShader.Use();
        
        glm::mat4 view;
        glm::mat4 projection;
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float) gameHeight/(float)gameWidth, 0.1f, 1000.0f);
        
        GLint modelLoc = glGetUniformLocation(mainShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(mainShader.Program, "view");
        GLint projectionLoc = glGetUniformLocation(mainShader.Program, "projection");
        
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(VAO);
        
        
        GLint isSafeLaneLoc = glGetUniformLocation(mainShader.Program, "isSafeLane");
        GLint isSafeLaneAfterSafeLane = glGetUniformLocation(mainShader.Program, "isSafeLaneAfterSafeLane");
        GLint isNormalLaneBeforeNormalLane = glGetUniformLocation(mainShader.Program, "isNormalLaneBeforeNormalLane");
        
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
        
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
void do_movement()
{
    
    if(keys[GLFW_KEY_UP])
        camera.ProcessKeyboard(FRONT, deltaTime);
    if(keys[GLFW_KEY_DOWN])
        camera.ProcessKeyboard(BACK, deltaTime);
    
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

GLint TextureFromFile(const char* path, string directory)
{
    //Generate texture ID and load texture data
    string filename = string(path);
    filename = directory + '/' + filename;
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}

