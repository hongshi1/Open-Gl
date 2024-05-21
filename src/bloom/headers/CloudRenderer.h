#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random> // 添加这个头文件

class CloudRenderer {
private:
    std::vector<float> particles; // 粒子的位置和其他属性
    std::vector<float> colors; // 粒子的颜色
    unsigned int VAO, VBO, VBOColor;
    int numParticles;
    float cloudSize;
    float turbulenceStrength;

public:
    CloudRenderer(int numParticles, float cloudSize, float turbulenceStrength)
        : numParticles(numParticles), cloudSize(cloudSize), turbulenceStrength(turbulenceStrength), VAO(0), VBO(0), VBOColor(0) {
        generateParticles();
        generateColors();
    }

    ~CloudRenderer() {
        cleanup();
    }

    void cleanup() {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &VBOColor);
            VAO = 0;
            VBO = 0;
            VBOColor = 0;
        }
    }

    void render(const Shader& shader) {
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &VBOColor);

        glBindVertexArray(VAO);

        // 顶点缓冲对象
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(float), particles.data(), GL_STATIC_DRAW);

        // 位置属性
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // 颜色缓冲对象
        glBindBuffer(GL_ARRAY_BUFFER, VBOColor);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);

        // 颜色属性
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, numParticles);
    glBindVertexArray(0);
}


private:
    void generateParticles() {
        particles.reserve(numParticles * 3);

        // 使用C++标准库中更现代的随机数生成方法
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-cloudSize / 2, cloudSize / 2);

        for (int i = 0; i < numParticles; ++i) {
            float x = dis(gen);
            float y = dis(gen);
            float z = dis(gen);
            
            // 添加湍流效应
            float turbulenceX = turbulenceStrength * (dis(gen) - 0.5f);
            float turbulenceY = turbulenceStrength * (dis(gen) - 0.5f);
            float turbulenceZ = turbulenceStrength * (dis(gen) - 0.5f);

            x += turbulenceX;
            y += turbulenceY;
            z += turbulenceZ;

            particles.push_back(x);
            particles.push_back(y);
            particles.push_back(z);
        }
    }

    void generateColors() {
        colors.reserve(numParticles * 3);

        for (int i = 0; i < numParticles; ++i) {
            // 在底部使用较深的颜色，顶部使用较浅的颜色
            float gradient = static_cast<float>(i) / static_cast<float>(numParticles);
            float r = 0.8f * gradient;
            float g = 0.8f * gradient;
            float b = 0.9f + 0.1f * gradient;

            colors.push_back(r);
            colors.push_back(g);
            colors.push_back(b);
        }
    }
//     void generateColors() {
//     colors.reserve(numParticles * 3);

//     for (int i = 0; i < numParticles; ++i) {
//         // 在底部使用白色，顶部透明
//         float gradient = static_cast<float>(i) / static_cast<float>(numParticles);
//         float r = 1.0f; // 红色分量设为1表示白色
//         float g = 1.0f; // 绿色分量设为1表示白色
//         float b = 1.0f; // 蓝色分量设为1表示白色

//         colors.push_back(r);
//         colors.push_back(g);
//         colors.push_back(b);
//     }
// }
};