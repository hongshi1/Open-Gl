#include "ConeRenderer.h"

ConeRenderer::ConeRenderer(int sectors, float radius, float height)
    : ShapeRenderer(3, sectors), sectors(sectors), radius(radius), height(height) {
    float sectorStep = 2 * glm::pi<float>() / sectors;

    // Vertices
    vertices.reserve((sectors + 1) * 3 * 3); // 3 vertices per face, 3 floats per vertex, plus the center vertex
    for (int i = 0; i <= sectors; ++i) {
        float sectorAngle = i * sectorStep;
        float x = radius * cosf(sectorAngle);
        float y = radius * sinf(sectorAngle);

        // Bottom triangle
        vertices.push_back(x); // Bottom vertex
        vertices.push_back(y);
        vertices.push_back(0.0f);

        vertices.push_back(0.0f); // Center
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        vertices.push_back(radius * cosf(sectorAngle + sectorStep)); // Next bottom vertex
        vertices.push_back(radius * sinf(sectorAngle + sectorStep));
        vertices.push_back(0.0f);

        // Side triangle
        vertices.push_back(0.0f); // Apex
        vertices.push_back(0.0f);
        vertices.push_back(height);
    }

    // Indices
    indices.reserve(sectors * 3 * 2); // 3 vertices per face, 2 faces per sector
    for (int i = 0; i < sectors; ++i) {
        int index = i * 12; // Each vertex has 3 components

        indices.push_back(index);     // Bottom vertex
        indices.push_back(index + 1); // Center
        indices.push_back(index + 4); // Next bottom vertex

        indices.push_back(index + 4); // Next bottom vertex
        indices.push_back(index + 1); // Center
        indices.push_back(index + 7); // Apex

        indices.push_back(index + 4); // Next bottom vertex
        indices.push_back(index + 1); // Center
        indices.push_back(index + 10); // Next top vertex
    }
}

void ConeRenderer::render() {
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
