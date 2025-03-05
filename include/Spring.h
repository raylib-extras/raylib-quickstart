#pragma once
#include "Particle.h"

class Spring {
public:
    Particle* p1, * p2;
    float restLength;
    float stiffness; // the larger, the stronger

    Spring(Particle* a, Particle* b, float k);
    void ApplySpringForce();
};
