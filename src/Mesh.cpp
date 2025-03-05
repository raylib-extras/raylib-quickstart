#include "Mesh.h"
#include "raylib.h"

const float SNIFF = 50.0f;
const float HEIGHT = 0.0f;

namespace mesh3d {
    Mesh::Mesh(int w, int h, float spacing) : width(w), height(h) {
        const Vector3 ORIGIN = { w * spacing / 2, HEIGHT, h * spacing / 2 };

        // Create particles
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                bool fixed = (y == 0 || x == 0 || y == h - 1 || x == w - 1);  // Fix top row
                particles.emplace_back(Vector3{ x * spacing - ORIGIN.x, 0 - ORIGIN.y, y * spacing - ORIGIN.z }, fixed);
            }
        }

        // Create springs
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int idx = y * w + x;
                if (x < w - 1) springs.emplace_back(&particles[idx], &particles[idx + 1], SNIFF); // Horizontal
                if (y < h - 1) springs.emplace_back(&particles[idx], &particles[idx + w], SNIFF); // Vertical
            }
        }
    }

    void Mesh::Update(float dt) {
		if (dt <= 0.0f) return;

        for (auto& spring : springs) {
            spring.ApplySpringForce(); // hook law
        }
        for (auto& particle : particles) {
            particle.ApplyForce(Vector3{ 0, -9.8f, 0 });  // Gravity
			// air resistance
			const float AIR_RESISTANCE = -0.1f;
			particle.ApplyForce(Vector3{ AIR_RESISTANCE * particle.velocity.x, AIR_RESISTANCE * particle.velocity.y, AIR_RESISTANCE * particle.velocity.z });
            particle.Update(dt);
        }
    }

    void Mesh::Draw() {

        for (auto& spring : springs) {
            DrawLine3D(spring.p1->position, spring.p2->position, BLUE);
        }

		for (auto& particle : particles) {
			DrawSphere(particle.position, 0.1f, RED);
		}
    }
}
