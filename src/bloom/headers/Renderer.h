#pragma once
#include <cassert>
#include <glad/glad.h>

#define ASSERT(x) \
    if (!(x))     \
        assert(false);
#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char *function, const char *file, int line);

class Renderer
{
public:
    void Draw(int numIndices) const;
};
