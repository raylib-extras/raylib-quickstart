#include "Spring.h"
#include <cmath>

Spring::Spring(Particle* a, Particle* b, float k)
    : p1(a), p2(b), stiffness(k) {
    restLength = sqrt(pow(b->position.x - a->position.x, 2) +pow(b->position.y - a->position.y, 2) +pow(b->position.z - a->position.z, 2));
}

void Spring::ApplySpringForce() {
	// point from p1 to p2. meaning p1 is the origin
    const Vector3 diff = { p2->position.x - p1->position.x,
                     p2->position.y - p1->position.y,
                     p2->position.z - p1->position.z };

    float length = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

    const float forceMagnitude = stiffness * (length - restLength);



    const Vector3 force = { forceMagnitude * (diff.x / length),
                      forceMagnitude * (diff.y / length),
					  forceMagnitude* (diff.z / length) };

	p1->ApplyForce(force); // p1 is origin
    p2->ApplyForce({ -force.x, -force.y, -force.z });  // Apply opposite force
}
