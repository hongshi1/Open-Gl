#include "ShapeRenderer.h"

ShapeRenderer::ShapeRenderer(int verticesPerFace, int faces)
    : VAO(0), VBO(0), EBO(0), verticesPerFace(verticesPerFace), faces(faces) {}

// Destructor to release OpenGL resources
ShapeRenderer::~ShapeRenderer() {
    if (VBO) {
        glDeleteBuffers(1, &VBO);
    }
    if (EBO) {
        glDeleteBuffers(1, &EBO);
    }
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
    }
}
