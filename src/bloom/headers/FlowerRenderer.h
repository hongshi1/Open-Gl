#ifndef FlowerRenderer_H
#define FlowerRenderer_H

#include "ShapeRenderer.h"

class FlowerRenderer : public ShapeRenderer {
public:
    FlowerRenderer(int sectors, float radius, float height);
    ~FlowerRenderer(); // Add destructor
    void render() override;

private:
    void cleanup(); // Add cleanup method
    int sectors;
    float radius;
    float height;
};

#endif
