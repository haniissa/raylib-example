#pragma once
#include <raylib.h>
#include <vector>

struct Bullet {
	Vector3 position;
	Vector3 velocity;
	float   size = 0.4F;
	bool 	active = false;
};

struct BulletPool {
	std::vector<Bullet> bullets;
	size_t     maxBullets = 64;

	BulletPool();
	void Spawn(Vector3 origin, Vector3 direction, float speed);
	void Update(float dt, float halfArena);
	void Draw() const;
};
