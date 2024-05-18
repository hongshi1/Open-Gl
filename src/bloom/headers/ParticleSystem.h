#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// 粒子结构体
struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float life;
};

// 粒子系统类
class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    void initialize(); // 初始化粒子系统
    void update(float dt); // 更新粒子系统
    void render(); // 渲染粒子系统

private:
    bool initialized;
    unsigned int particleVAO, particleVBO;
    unsigned int maxParticles;
    std::vector<Particle> particles; // 存储所有粒子的容器

    void respawnParticle(Particle& particle); // 重生粒子

    float rotationAngle; // 用于表示旋转角度
};

#endif // PARTICLE_SYSTEM_H
