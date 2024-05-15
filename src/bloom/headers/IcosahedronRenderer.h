#ifndef ICOSAHEDRON_RENDERER_H
#define ICOSAHEDRON_RENDERER_H

#include <glad/glad.h>

class IcosahedronRenderer {
public:
    IcosahedronRenderer();
    void renderIcosahedron();

private:
    bool initialized;
    unsigned int icosahedronVAO, icosahedronVBO, icosahedronEBO;
};

#endif // ICOSAHEDRON_RENDERER_H
