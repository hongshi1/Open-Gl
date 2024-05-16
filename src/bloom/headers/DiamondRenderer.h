#ifndef DIAMONDRENDERER_H
#define DIAMONDRENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class DiamondRenderer {
private:
    static bool initialized;
    static unsigned int diamondVAO, diamondVBO, diamondEBO;

public:
    DiamondRenderer();
    void renderDiamond();
};

#endif
