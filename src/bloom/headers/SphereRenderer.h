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
    // 构造函数
    SphereRenderer();

    // 析构函数
    ~SphereRenderer();

    // 渲染球体的方法
    void renderSphere();

    // 清理资源的方法
    void cleanup();
};

#endif // SPHERERENDERER_H