#pragma once
#include <raylib.h>

struct Player{
	Vector3 position{0.0F, 0.0F, 0.0F};
	float size{1.5F}; // half-extent
	float speed{12.0F};
	int   lives{3};

	void Update(float dt, Vector3 moveDir, float halfArena);
	void Draw() const;
	void Respawn();
};
