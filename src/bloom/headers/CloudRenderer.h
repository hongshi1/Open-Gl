#ifndef CLOUD_RENDERER_H
#define CLOUD_RENDERER_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "ParticleSystem.h"


class CloudRenderer {
public:
    CloudRenderer();
    ~CloudRenderer();

    void initialize(float maxX, float maxY, float maxZ);
    void update(float dt, float maxX, float maxY, float maxZ);
    void render(float maxX, float maxY, float maxZ);

private:
    void respawnParticle(Particle& particle, float maxX, float maxY, float maxZ);

    bool initialized;
    GLuint particleVAO;
    GLuint particleVBO;
    const int maxParticles;
    std::vector<Particle> particles;
    float rotationAngle;
};

#endif // CLOUD_RENDERER_H
