#pragma once
#include <raylib.h>
#include <vector>

struct Enemy{
	Vector3 position;
	Vector3 velocity;
	float   size = 1.2F;
	bool    active = true;
	Color   color;

	void Update(float dt, float halfArena);
	void Draw() const;
};

struct EnemyManager{
	EnemyManager();
	std::vector<Enemy> enemies;
	int  			   maxEnemies = 12;
	void SpawnAll(float halfArena, int count);
	void Update(float dt, float halfArena);
	void Draw() const;
	// void Spawn();
};
