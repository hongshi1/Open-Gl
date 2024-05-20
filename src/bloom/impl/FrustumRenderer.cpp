#include "FrustumRenderer.h"

FrustumRenderer::FrustumRenderer(int sectors, float bottomRadius, float topRadius, float height)
    : ShapeRenderer(3, sectors), sectors(sectors), bottomRadius(bottomRadius), topRadius(topRadius), height(height) {
    float sectorStep = 2 * glm::pi<float>() / sectors;

    // Vertices
    vertices.reserve((sectors + 1) * 2 * 3 + 2 * 3); // 2 circles with (sectors+1) vertices each, plus 2 center vertices
    for (int i = 0; i <= sectors; ++i) {
        float sectorAngle = i * sectorStep;

        // Bottom circle
        float x1 = bottomRadius * cosf(sectorAngle);
        float z1 = bottomRadius * sinf(sectorAngle);
        vertices.push_back(x1);
        vertices.push_back(0.0f); // Bottom circle at y = 0
        vertices.push_back(z1);

        // Top circle
        float x2 = topRadius * cosf(sectorAngle);
        float z2 = topRadius * sinf(sectorAngle);
        vertices.push_back(x2);
        vertices.push_back(height); // Top circle at y = height
        vertices.push_back(z2);
    }

    // Center vertex for bottom circle
    vertices.push_back(0.0f);
    vertices.push_back(0.0f); // Bottom center at y = 0
    vertices.push_back(0.0f);

    // Center vertex for top circle
    vertices.push_back(0.0f);
    vertices.push_back(height); // Top center at y = height
    vertices.push_back(0.0f);

    // Indices
    indices.reserve(sectors * 6 + sectors * 3 * 2); // 6 vertices per sector for sides, 3 vertices per sector for top and bottom

    for (int i = 0; i < sectors; ++i) {
        int current = i * 2;
        int next = (i + 1) * 2;

        // Side faces
        indices.push_back(current);
        indices.push_back(next);
        indices.push_back(current + 1);

        indices.push_back(next);
        indices.push_back(next + 1);
        indices.push_back(current + 1);
    }

    int bottomCenterIndex = (sectors + 1) * 2;
    int topCenterIndex = bottomCenterIndex + 1;

    for (int i = 0; i < sectors; ++i) {
        int next = (i + 1) % sectors;

        // Bottom circle
        indices.push_back(bottomCenterIndex);
        indices.push_back(i * 2);
        indices.push_back(next * 2);

        // Top circle
        indices.push_back(topCenterIndex);
        indices.push_back(next * 2 + 1);
        indices.push_back(i * 2 + 1);
    }
}

void FrustumRenderer::render() {
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
