#include "Particle.h"

Particle::Particle(Vector3 pos, bool fixed) : position(pos), isFixed(fixed) {}

void Particle::ApplyForce(Vector3 f) {
    if (!isFixed) force = { force.x + f.x, force.y + f.y, force.z + f.z };
}

void Particle::Update(float dt) {
    if (isFixed || dt <= 0.0f) return;

    Vector3 acceleration = { force.x / mass, force.y / mass, force.z / mass };

    velocity = { velocity.x + acceleration.x * dt, velocity.y + acceleration.y * dt, velocity.z + acceleration.z * dt };
    position = { position.x + velocity.x * dt, position.y + velocity.y * dt, position.z + velocity.z * dt };

    force = { 0, 0, 0 };  // Reset force for next frame
}
