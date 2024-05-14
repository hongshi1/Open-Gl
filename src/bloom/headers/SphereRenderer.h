#ifndef SPHERERENDERER_H
#define SPHERERENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class SphereRenderer {
private:
    bool initialized;
    unsigned int sphereVAO, sphereVBO, sphereEBO;
    const int sectors;
    const int stacks;

public:
    SphereRenderer();
    void renderSphere();
};

#endif
