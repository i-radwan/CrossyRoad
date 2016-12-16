#ifndef PEN_MODEL
#define PEN_MODEL

#include "PenMesh.h"
#include <iostream>
#include <string>
#include <vector>
#include <utility>

using namespace std;
vector<Texture> textures_loaded;

class PenModel
{
public:
    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    PenModel(GLchar* path)
    {
        this->loadModel(path);
    }
    
    // Draws the model, and thus all its meshes
    void Draw(Shader shader, GLuint frameCount, glm::mat4 objmodel, bool moving)
    {
        for(GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].Draw(shader, this->modelAnimations, frameCount, objmodel, moving);
    }
    void Render(Shader &depthShader, GLuint frameCount, glm::mat4 objmodel, bool moving){
        for(GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].Render(depthShader, this->modelAnimations, frameCount, objmodel, moving);
    }
    
    vector<pair< string, vector<animationData> > > getModelAnimations(){return this->modelAnimations;}
private:
    /*  Model Data  */
    vector<PenMesh> meshes;
    string directory;
    
    vector<pair< string, vector<animationData> > > modelAnimations; // animation storage
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
        
        // Get animations
        for (int i = 0; i<scene->mNumAnimations; i++) {
            aiAnimation* animation = scene->mAnimations[i];

            string nodeName =animation->mName.C_Str();
//            cout << "Animation: " << animation->mName.C_Str() << " " << animation->mDuration << endl;
            
            for (int j = 0; j< animation->mNumChannels; j++) {
                aiNodeAnim* nodeAnimation = animation->mChannels[j];
//                cout << "Node: " << nodeAnimation->mNodeName.C_Str() << endl;
                
                string nodeName = nodeAnimation->mNodeName.C_Str() ;
                vector<animationData> animData(nodeAnimation->mNumPositionKeys);
            
                for (int k = 0; k < nodeAnimation->mNumPositionKeys; k++) {
                    aiVectorKey position = nodeAnimation->mPositionKeys[k];
                    animData[k].time = position.mTime;
                    animData[k].position.x = position.mValue.x;
                    animData[k].position.y = position.mValue.y;
                    animData[k].position.z = position.mValue.z;
//                    cout << "Position: Time-> " << position.mTime << " x: " << position.mValue.x <<  " y: " << position.mValue.y << " z: "<<  position.mValue.z << endl;
                }
                for (int k = 0; k < nodeAnimation->mNumRotationKeys; k++) {
                    aiQuatKey rotating = nodeAnimation->mRotationKeys[k];
                    animData[k].rotation.x = rotating.mValue.x;
                    animData[k].rotation.y = rotating.mValue.y;
                    animData[k].rotation.z = rotating.mValue.z;
//                    cout << "Rotating: Time-> " << rotating.mTime << " x: " << rotating.mValue.x <<  " y: " << rotating.mValue.y << " z: " << rotating.mValue.z<< " w: " << rotating.mValue.w << endl;
                }
                for (int k = 0; k < nodeAnimation->mNumScalingKeys; k++) {
                    aiVectorKey scale = nodeAnimation->mScalingKeys[k];
                    animData[k].scaling.x = scale.mValue.x;
                    animData[k].scaling.y = scale.mValue.y;
                    animData[k].scaling.z = scale.mValue.z;
//                    cout << "Scale: Time-> " << scale.mTime << " x: " << scale.mValue.x <<  " y: "<<  scale.mValue.y << " z: "<<  scale.mValue.z << endl;
                }
                modelAnimations.push_back(make_pair(nodeName, animData));
                cout << endl<<endl;
            }
            cout << endl<<endl;
            cout << endl<<endl;
        }
        
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
            this->meshes.push_back(this->processMesh(mesh, scene, node->mName.C_Str()));
        }
        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(GLuint i = 0; i < node->mNumChildren; i++)
        {
            this->processNode(node->mChildren[i], scene);
        }
        
    }
    
    PenMesh processMesh(aiMesh* mesh, const aiScene* scene, string nodeName)
    {
        // Data to fill
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        ObjColor objColor;
        
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
            vertex.Normal = vector;            // Texture Coordinates
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
            
            aiColor4D diffuse;
            aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
            objColor.diffColor = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
            //            std::cout << "Mat. Diff: " << diffuse.r << " " << diffuse.g << " " << diffuse.b << " " << diffuse.a << endl;
            aiColor4D ambient;
            aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient);
            objColor.ambiColor = glm::vec4(ambient.r, ambient.g, ambient.b, ambient.a);
            
            //            std::cout << "Mat. Amb: " << ambient.r << " " << ambient.g << " " << ambient.b << " " << ambient.a << endl;
            aiColor4D specular;
            aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
            objColor.specColor = glm::vec4(specular.r, specular.g, specular.b, specular.a);
            
            //            std::cout << "Mat. Spec: " << specular.r << " " << specular.g << " " << specular.b << " " << specular.a << endl;
            
            aiColor4D emissive;
            aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &emissive);
            objColor.emiColor = glm::vec4(emissive.r, emissive.g, emissive.b, emissive.a);
            
            //            std::cout << "Mat. Emi: " << emissive.r << " " << emissive.g << " " << emissive.b << " " << emissive.a << endl;
            
            // 1. Diffuse maps
            vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. Specular maps
            vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        
        // Return a mesh object created from the extracted mesh data
        
        return PenMesh(vertices, indices, textures, objColor, nodeName);
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
};
#endif
