#ifndef FRUSTUMRENDERER_H
#define FRUSTUMRENDERER_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "ShapeRenderer.h"

class FrustumRenderer : public ShapeRenderer {
public:
    FrustumRenderer(int sectors, float bottomRadius, float topRadius, float height);
    void render() override;

private:
    int sectors;
    float bottomRadius;
    float topRadius;
    float height;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO = 0, VBO = 0, EBO = 0;
};

#endif // FRUSTUMRENDERER_H
