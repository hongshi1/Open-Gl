#pragma once
#include <assimp/Importer.hpp> //assimp库头文件
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/glad.h> // 所有头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex{
    glm::vec3 Position;     //位置向量
    glm::vec3 Normal;       //法向量
    glm::vec2 TexCoords;    //纹理坐标
};

struct Texture{
    unsigned int id;         //纹理材料数据：镜面反射/漫反射
    string type;
    aiString path;
};

class Mesh{
    public:
        /*网格数据*/
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        /*函数*/
        Mesh(vector<Vertex> vertices,vector<unsigned int> indices,vector<Texture> textures);
        void Draw(Shader shader);
    private:
        /*渲染数据*/
        unsigned int VAO, VBO, EBO;
        /*函数*/
        void setupMesh();
};