#ifndef SHAPERENDERER_H
#define SHAPERENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <string>
#include <vector>

class ShapeRenderer {
public:
    ShapeRenderer(int verticesPerFace, int faces);
    virtual ~ShapeRenderer();
    virtual void render() = 0;

protected:
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    int verticesPerFace;
    int faces;
};

#endif
