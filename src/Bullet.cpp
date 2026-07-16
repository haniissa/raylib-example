#include "Bullet.h"
#include "Physics.h"
#include <raylib.h>
#include <raymath.h>

BulletPool::BulletPool(){
	bullets.resize(maxBullets);
}

void BulletPool::Spawn(Vector3 origin, Vector3 direction, float speed){
	for(auto& b: bullets){
		if(!b.active){
			b.position = origin;
			b.velocity = origin;
			b.velocity = Vector3Scale(Vector3Normalize(direction), speed);
			b.active = true;
			return;
		}
	}
	// pool exhausted  - overwirte oldest (optional: ring buffer)
}


void BulletPool::Update(float dt, float halfArena){
	for(auto& b:bullets){
		if(!b.active) continue;
		b.position = Vector3Add(b.position, Vector3Scale(b.velocity, dt));
		if(Physics::IsOutOfBounds(b.position, halfArena))
			b.active = false;
	}
}

void BulletPool::Draw() const {
	for(auto& b:bullets){
		if(b.active)
			DrawCubeV(b.position, {b.size, b.size, b.size}, YELLOW);
	}
}
