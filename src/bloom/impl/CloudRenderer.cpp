#include "CloudRenderer.h"

CloudRenderer::CloudRenderer()
    : initialized(false), particleVAO(0), particleVBO(0), maxParticles(10000), particles(maxParticles), rotationAngle(0.0f) {
    srand(static_cast<unsigned int>(time(0)));
}

void CloudRenderer::initialize(float maxX, float maxY, float maxZ) {
    if (initialized)
        return;

    for (auto& particle : particles) {
        respawnParticle(particle, maxX, maxY, maxZ);
    }

    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);

    glBindVertexArray(particleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    initialized = true;
}

void CloudRenderer::respawnParticle(Particle& particle, float maxX, float maxY, float maxZ) {
    // 随机生成粒子的初始位置，限制在给定的范围内
    float x = ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * maxX;
    float y = ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * maxY;
    float z = ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * maxZ;

    particle.position = glm::vec3(x, y, z);
    particle.velocity = glm::vec3(0.0f);

    // 随机设置粒子的颜色
    particle.color = glm::vec4(static_cast<float>(rand()) / RAND_MAX,
                                static_cast<float>(rand()) / RAND_MAX,
                                static_cast<float>(rand()) / RAND_MAX,
                                1.0f);

    // 随机设置粒子的生命周期
    particle.life = ((static_cast<float>(rand()) / RAND_MAX) * 0.5f) + 0.5f; // 在[0.5, 1.0]范围内随机
}

void CloudRenderer::update(float dt, float maxX, float maxY, float maxZ) {
    for (auto& particle : particles) {
        // Update particle position
        particle.position += particle.velocity * dt;

        // Handle collision with container walls
        if (particle.position.x < 0 || particle.position.x >= maxX) {
            particle.velocity.x *= -1.0f; // Reverse velocity in X direction
        }
        if (particle.position.y < 0 || particle.position.y >= maxY) {
            particle.velocity.y *= -1.0f; // Reverse velocity in Y direction
        }
        if (particle.position.z < -5.0f || particle.position.z >= maxZ) {
            particle.velocity.z *= -1.0f; // Reverse velocity in Z direction
        }

        // Add turbulence effect
        glm::vec3 turbulence(
            ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 0.2f,
            ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 0.2f,
            ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 0.2f
        );
        particle.velocity += turbulence;

        // Limit maximum velocity to avoid particles from flying too fast
        particle.velocity = glm::normalize(particle.velocity) * 1.0f;

        // Decrease particle life
        particle.life -= dt;
        if (particle.life <= 0.0f) {
            respawnParticle(particle, maxX, maxY, maxZ);
        }
    }
}

void CloudRenderer::render(float maxX, float maxY, float maxZ) {
    initialize(maxX, maxY, maxZ);

    // Set point size for particles (optional)
    glPointSize(1.0f); // 调整点的大小

    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, maxParticles * sizeof(Particle), particles.data());

    glBindVertexArray(particleVAO);
    glDrawArrays(GL_POINTS, 0, maxParticles);
    glBindVertexArray(0);
}

CloudRenderer::~CloudRenderer() {
    if (initialized) {
        glDeleteVertexArrays(1, &particleVAO);
        glDeleteBuffers(1, &particleVBO);
    }
}
