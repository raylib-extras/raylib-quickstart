#pragma once
#include "raylib.h"

class Particle {
public:
    Vector3 position;
    Vector3 velocity = { 0.0f, 0.0f, 0.0f };
    Vector3 force;
	float mass = 0.01f;
    bool isFixed = false;  // Whether this particle is locked in place

    Particle(Vector3 pos, bool fixed = false);
    void ApplyForce(Vector3 f);
    void Update(float dt);
};
