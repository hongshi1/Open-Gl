#include "ShapeRenderer.h"

ShapeRenderer::ShapeRenderer(int verticesPerFace, int faces)
    : VAO(0), VBO(0), EBO(0), verticesPerFace(verticesPerFace), faces(faces) {}
