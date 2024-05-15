#include "IcosahedronRenderer.h"
#include <cmath>

IcosahedronRenderer::IcosahedronRenderer() : initialized(false) {}

void IcosahedronRenderer::renderIcosahedron() {
    if (!initialized) {
        float phi = (1 + sqrt(5.0f)) / 2.0f; // Golden ratio

        float icosahedronVertices[] = {
            -1, phi, 0, // Vertex 0
            1, phi, 0,  // Vertex 1
            -1, -phi, 0, // Vertex 2
            1, -phi, 0, // Vertex 3
            0, -1, phi, // Vertex 4
            0, 1, phi,  // Vertex 5
            0, -1, -phi, // Vertex 6
            0, 1, -phi, // Vertex 7
            phi, 0, -1, // Vertex 8
            phi, 0, 1,  // Vertex 9
            -phi, 0, -1, // Vertex 10
            -phi, 0, 1   // Vertex 11
        };

        unsigned int icosahedronIndices[] = {
            0, 11, 5, 0, 5, 1,
            0, 1, 7, 0, 7, 10,
            0, 10, 11, 1, 5, 9,
            5, 11, 4, 11, 10, 2,
            10, 7, 6, 10, 6, 2,
            7, 1, 8, 1, 9, 8,
            3, 9, 4, 3, 4, 2,
            3, 2, 6, 3, 6, 8,
            3, 8, 9, 4, 9, 5,
            2, 4, 11, 6, 7, 8
        };

        glGenVertexArrays(1, &icosahedronVAO);
        glGenBuffers(1, &icosahedronVBO);
        glGenBuffers(1, &icosahedronEBO);

        glBindVertexArray(icosahedronVAO);

        glBindBuffer(GL_ARRAY_BUFFER, icosahedronVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(icosahedronVertices), icosahedronVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, icosahedronEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(icosahedronIndices), icosahedronIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        initialized = true;
    }

    glBindVertexArray(icosahedronVAO);
    glDrawElements(GL_TRIANGLES, 20 * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
