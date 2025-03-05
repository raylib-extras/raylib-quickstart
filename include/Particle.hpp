#pragma once
#include "raylib.h"

class Particle {
public:
    Vector3 position;
    Vector3 velocity;
    Vector3 force;
    bool isFixed = false;  // Whether this particle is locked in place

    Particle(Vector3 pos, bool fixed = false);
    void ApplyForce(Vector3 f);
    void Update(float dt);
};
