#pragma once
#include <vector>
#include "Particle.h"
#include "Spring.h"

namespace mesh3d{
    class Mesh {
    public:
        int width, height;
        std::vector<Particle> particles;
        std::vector<Spring> springs;

        Mesh(int w, int h, float spacing);
        void Update(float dt);
        void Draw();
    };
} // namespace mesh3d
