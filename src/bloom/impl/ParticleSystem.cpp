#include "ParticleSystem.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>


// 默认生成1000个粒子
ParticleSystem::ParticleSystem()
    : initialized(false), particleVAO(0), particleVBO(0), maxParticles(10000), particles(maxParticles),rotationAngle(0.0f)  {
    srand(static_cast<unsigned int>(time(0)));
}

void ParticleSystem::initialize() {
    if (initialized) return;

    for (auto& particle : particles) {
        respawnParticle(particle);
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

void ParticleSystem::respawnParticle(Particle& particle) {
    float u = static_cast<float>(rand()) / RAND_MAX;  // 随机生成 [0, 1] 之间的浮点数
    float v = static_cast<float>(rand()) / RAND_MAX;  // 随机生成 [0, 1] 之间的浮点数

    // 根据参数方程计算椭球体上的点
    float theta = 2.0f * glm::pi<float>() * u;
    float phi = glm::acos(2.0f * v - 1.0f);

    float radius = 5.0f;  // 椭球体的半径
    float x = radius * glm::sin(phi) * glm::cos(theta);
    float y = radius * glm::sin(phi) * glm::sin(theta);
    float z = radius * glm::cos(phi);

    // 使用生成的点设置粒子的位置
    particle.position = glm::vec3(x, y, z);

    // 设置粒子的初始速度（例如，以恒定速度向上移动）
    particle.velocity = glm::vec3(0.0f, 0.8f, 0.0f);

    // 将粒子的颜色设置为白色并且完全不透明
    particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // 将粒子的生命值设置为 1.0（表示它是完全存活的）
    particle.life = 1.0f;
}


// void ParticleSystem::respawnParticle(Particle& particle) {
//     // 生成范围在 (-5.0, 5.0) 之间的随机 X 位置
//     //rand() % 100 取模运算将随机整数限制在 0 到 99 之间
//     //(rand() % 100) - 50 一个范围在 -50 到 49 之间的整数
//     float randomX = ((rand() % 1000) - 500) / 10.0f;
    
//     // 生成范围在 (-5.0, 5.0) 之间的随机 Y 位置
//     float randomY = ((rand() % 1000) - 500) / 10.0f;
    
//     // 使用生成的随机坐标设置粒子的位置
//     particle.position = glm::vec3(randomX, randomY, 0.0f);
    
//     // 设置粒子的初始速度（例如，以恒定速度向上移动）
//     particle.velocity = glm::vec3(0.0f, 0.1f, 0.0f);
    
//     // 将粒子的颜色设置为白色并且完全不透明
//     particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    
//     // 将粒子的生命值设置为 1.0（表示它是完全存活的）
//     particle.life = 1.0f;
// }


// 粒子平移
void ParticleSystem::update(float dt) {
    for (auto& particle : particles) {
        particle.life -= dt;
        if (particle.life > 0.0f) {
            particle.position += particle.velocity * dt;
            particle.color.a -= dt;
        } else {
            respawnParticle(particle);
        }
    }
}

// 粒子旋转
// void  ParticleSystem::update(float dt) {
//         // 每一帧更新旋转角度
//         rotationAngle += 0.01f;

//         for (auto&particle : particles) {
//             particle.life -= dt;
//             if (particle.life>0.0f) {
//                 // 在更新粒子位置之前，应用旋转变换
//                 float x = particle.position.x;
//                 float z = particle.position.z;
//                 particle.position.x = x * glm::cos(rotationAngle) - z * glm::sin(rotationAngle);
//                 particle.position.z = x * glm::sin(rotationAngle) + z * glm::cos(rotationAngle);

//                 particle.position += particle.velocity * dt;
//                 particle.color.a -= dt;
//             } else {
//                 respawnParticle(particle);
//             }
//         }
// }

void ParticleSystem::render() {
    initialize();

    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, maxParticles * sizeof(Particle), particles.data());

    glBindVertexArray(particleVAO);
    glDrawArrays(GL_POINTS, 0, maxParticles);
    glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem() {
    if (initialized) {
        glDeleteVertexArrays(1, &particleVAO);
        glDeleteBuffers(1, &particleVBO);
    }
}
