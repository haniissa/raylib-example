#include "Enemy.h"
#include <algorithm>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <fmt/base.h>
#include <cstdlib>

EnemyManager::EnemyManager(){
	enemies.resize(maxEnemies);
}

void EnemyManager::Update(float dt, float halfArena){
	for(auto& e:enemies)
		e.Update(dt,  halfArena);
}

void EnemyManager::Draw()const{
	for(const auto& e:enemies)
		e.Draw();
}

void EnemyManager::SpawnAll(float halfArena, int count){
	for(int i{}; i < count && i < maxEnemies; ++i){
		auto& e = enemies[i];
		float x =(float)GetRandomValue(-(int)halfArena + 2, (int)halfArena - 2);
		float z = (float)GetRandomValue(-(int)halfArena + 2, (int)halfArena - 2);
		e.position = {x, 0.0F, z};
		e.velocity = {(float)GetRandomValue(-5, 5), 0, (float)GetRandomValue(-5, 5)};
		e.active = true;
		e.color = Color{(unsigned char)GetRandomValue(100, 255),
			(unsigned char)GetRandomValue(50, 150),
			(unsigned char)GetRandomValue(50, 150), 255};
		e.size = 0.9F + (float)GetRandomValue(0, 60) / 100.0F;
	}
	fmt::println("[Enemy] Spawned {} enemies", count);
}

void Enemy::Update(float dt, float halfArena){
	if(!active)return;
	position = Vector3Add(position, Vector3Scale(velocity, dt));
	//bounce off walls
	if(std::fabs(position.x) > halfArena - size){
		velocity.x *= -1;
		position.x = std::clamp(position.x, -halfArena + size, halfArena - size);
	}

	if (std::fabs(position.z) > halfArena - size) {
	 velocity.z *= -1;
		position.z = std::clamp(
			position.z,
			-halfArena + size,
			halfArena - size);
	}
}

void Enemy::Draw() const {
	if(!active) return;
	DrawCubeV(position, {size, size, size}, color);
	DrawCubeWiresV(position, {size, size, size}, ColorAlpha(color, 0.3F));
}
