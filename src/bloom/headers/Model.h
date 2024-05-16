#pragma once
#include <assimp/Importer.hpp> //assimp库头文件
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
using namespace std;
class Model{
    public:
        /*构造器*/
        Model(char *path)
        {
            loadModel(path);
        }
        void Draw(Shader shader);
    private:
    vector<Texture> textures_loaded;
        /*模型数据*/
        vector<Mesh> meshes;
        string directory;
        /*函数*/
        void loadModel(string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                             string typeName);
};