#include "DiamondRenderer.h"

bool DiamondRenderer::initialized = false;
unsigned int DiamondRenderer::diamondVAO = 0;
unsigned int DiamondRenderer::diamondVBO = 0;
unsigned int DiamondRenderer::diamondEBO = 0;

DiamondRenderer::DiamondRenderer() {}

void DiamondRenderer::renderDiamond() {
    // Initialize (if necessary)
    if (diamondVAO == 0) {
        // Define vertices with shared positions
        float octahedronVertices[] = {
            // Positions            // Normals            // Texture coords
            // Front face
            -0.5f, 0.0f, 0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // 0
            0.5f, 0.0f, 0.5f,      0.0f, 1.0f, 0.0f,    1.0f, 0.0f,  // 1
            0.0f, 0.5f, 0.0f,      0.0f, 1.0f, 0.0f,    0.5f, 1.0f,  // 2

            // Right face
            0.5f, 0.0f, 0.5f,      1.0f, 0.0f, 0.0f,    0.0f, 0.0f,  // 3
            0.5f, 0.0f, -0.5f,     1.0f, 0.0f, 0.0f,    1.0f, 0.0f,  // 4
            0.0f, 0.5f, 0.0f,      1.0f, 0.0f, 0.0f,    0.5f, 1.0f,  // 5

            // Back face
            0.5f, 0.0f, -0.5f,     0.0f, 0.0f, -1.0f,   0.0f, 0.0f,  // 6
            -0.5f, 0.0f, -0.5f,    0.0f, 0.0f, -1.0f,   1.0f, 0.0f,  // 7
            0.0f, 0.5f, 0.0f,      0.0f, 0.0f, -1.0f,   0.5f, 1.0f,  // 8

            // Left face
            -0.5f, 0.0f, -0.5f,    -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // 9
            -0.5f, 0.0f, 0.5f,     -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // 10
            0.0f, 0.5f, 0.0f,      -1.0f, 0.0f, 0.0f,   0.5f, 1.0f,  // 11

            // Bottom face
            -0.5f, 0.0f, 0.5f,     0.0f, -1.0f, 0.0f,   0.0f, 0.0f,  // 12
            0.5f, 0.0f, 0.5f,      0.0f, -1.0f, 0.0f,   1.0f, 0.0f,  // 13
            0.0f, -0.5f, 0.0f,     0.0f, -1.0f, 0.0f,   0.5f, 1.0f,  // 14

            // Right face
            0.5f, 0.0f, 0.5f,      -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // 15
            0.5f, 0.0f, -0.5f,     -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // 16
            0.0f, -0.5f, 0.0f,     -1.0f, 0.0f, 0.0f,   0.5f, 1.0f,  // 17

            // Back face
            0.5f, 0.0f, -0.5f,     0.0f, -1.0f, 0.0f,   0.0f, 0.0f,  // 18
            -0.5f, 0.0f, -0.5f,    0.0f, -1.0f, 0.0f,   1.0f, 0.0f,  // 19
            0.0f, -0.5f, 0.0f,     0.0f, -1.0f, 0.0f,   0.5f, 1.0f,  // 20

            // Left face
            -0.5f, 0.0f, -0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,  // 21
            -0.5f, 0.0f, 0.5f,     1.0f, 0.0f, 0.0f,    1.0f, 0.0f,  // 22
            0.0f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,    0.5f, 1.0f   // 23
        };

        unsigned int indices[] = {
            0, 1, 2,    // Front face
            3, 4, 5,    // Right face
            6, 7, 8,    // Back face
            9, 10, 11,  // Left face
            12, 13, 14, // Bottom face
            15, 16, 17, // Right face
            18, 19, 20, // Back face
            21, 22, 23  // Left face
        };

        // Generate buffers
        glGenVertexArrays(1, &diamondVAO);
        glGenBuffers(1, &diamondVBO);
        glGenBuffers(1, &diamondEBO);

        // Bind VAO
        glBindVertexArray(diamondVAO);

        // Bind and fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, diamondVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(octahedronVertices), octahedronVertices, GL_STATIC_DRAW);

        // Bind and fill element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, diamondEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coords
        glEnableVertexAttribArray(2);

        // Unbind VAO
        glBindVertexArray(0);
    }

    // Render Diamond
    glBindVertexArray(diamondVAO);
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0); // 8 faces, 3 vertices per face
    glBindVertexArray(0);
}
