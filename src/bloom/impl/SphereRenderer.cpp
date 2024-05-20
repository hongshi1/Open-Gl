#include "SphereRenderer.h"
#include <cmath>
#include <iostream>

SphereRenderer::SphereRenderer() : initialized(false), sectors(36), stacks(18), sphereVAO(0), sphereVBO(0), sphereEBO(0) {}

// MeshGrid method to draw the sphere
void SphereRenderer::renderSphere() {
    if (!initialized) {
        std::cout << "-------------------------------------------" << std::endl;
        std::cout << "Initializing sphere..." << std::endl;
        const int numVertices = (sectors + 1) * (stacks + 1);
        const int numIndices = sectors * stacks * 6;
        float* sphereVertices = new float[numVertices * 8]; // 8 floats per vertex (3 positions + 3 normals + 2 texture coordinates)
        unsigned int* sphereIndices = new unsigned int[numIndices];

        float radius = 0.5f;
        float sectorStep = 2 * glm::pi<float>() / sectors;
        float stackStep = glm::pi<float>() / stacks;
        int count = 0;

        for (int i = 0; i <= stacks; ++i) {
            float stackAngle = glm::pi<float>() / 2 - i * stackStep;
            float xy = radius * cosf(stackAngle);
            float z = radius * sinf(stackAngle);

            for (int j = 0; j <= sectors; ++j) {
                float sectorAngle = j * sectorStep;

                float x = xy * cosf(sectorAngle);
                float y = xy * sinf(sectorAngle);

                // Vertex positions
                sphereVertices[count++] = x;
                sphereVertices[count++] = y;
                sphereVertices[count++] = z;

                // Normals
                sphereVertices[count++] = x / radius;
                sphereVertices[count++] = y / radius;
                sphereVertices[count++] = z / radius;

                // Texture coordinates
                sphereVertices[count++] = static_cast<float>(j) / sectors;
                sphereVertices[count++] = static_cast<float>(i) / stacks;
            }
        }

        count = 0;
        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < sectors; ++j) {
                int first = (i * (sectors + 1)) + j;
                int second = first + sectors + 1;

                // Indices for two triangles
                sphereIndices[count++] = first;
                sphereIndices[count++] = second;
                sphereIndices[count++] = first + 1;

                sphereIndices[count++] = second;
                sphereIndices[count++] = second + 1;
                sphereIndices[count++] = first + 1;
            }
        }

        // Generate buffers
        glGenVertexArrays(1, &sphereVAO);
        glGenBuffers(1, &sphereVBO);
        glGenBuffers(1, &sphereEBO);

        // Bind VAO
        glBindVertexArray(sphereVAO);

        // Bind and fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, numVertices * 8 * sizeof(float), sphereVertices, GL_STATIC_DRAW);

        // Bind and fill element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), sphereIndices, GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coords
        glEnableVertexAttribArray(2);

        // Unbind VAO
        glBindVertexArray(0);

        // Clean up dynamically allocated arrays
        delete[] sphereVertices;
        delete[] sphereIndices;

        initialized = true;
    }

    // Render Sphere
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sectors * stacks * 6, GL_UNSIGNED_INT, 0); // 6 indices per face, sectors * stacks faces
    glBindVertexArray(0);
}

SphereRenderer::~SphereRenderer() {
    cleanup();
}

void SphereRenderer::cleanup() {
    if (initialized) {
        glDeleteVertexArrays(1, &sphereVAO);
        glDeleteBuffers(1, &sphereVBO);
        glDeleteBuffers(1, &sphereEBO);
        initialized = false;
    }
}
