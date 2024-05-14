#ifndef CONERENDERER_H
#define CONERENDERER_H

#include "ShapeRenderer.h"

class ConeRenderer : public ShapeRenderer {
public:
    ConeRenderer(int sectors, float radius, float height);
    void render() override;

private:
    int sectors;
    float radius;
    float height;
};

#endif
