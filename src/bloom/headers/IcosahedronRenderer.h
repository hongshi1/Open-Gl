#ifndef ICOSAHEDRON_RENDERER_H
#define ICOSAHEDRON_RENDERER_H

#include <glad/glad.h>

class IcosahedronRenderer {
public:
    IcosahedronRenderer();
    ~IcosahedronRenderer();
    void renderIcosahedron();

private:
    void cleanup();
    bool initialized;
    unsigned int icosahedronVAO, icosahedronVBO, icosahedronEBO;
};

#endif // ICOSAHEDRON_RENDERER_H
