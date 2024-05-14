#ifndef FlowerRenderer_H
#define FlowerRenderer_H

#include "ShapeRenderer.h"

class FlowerRenderer : public ShapeRenderer {
public:
    FlowerRenderer(int sectors, float radius, float height);
    void render() override;

private:
    int sectors;
    float radius;
    float height;
};

#endif
