#include "Player.h"
#include "Physics.h"
#include <fmt/base.h>
#include <raylib.h>
#include <raymath.h>


void Player::Update(float dt, Vector3 moveDir, float halfArena){
	moveDir = Vector3Scale(Vector3Normalize(moveDir), speed * dt);
	position = Vector3Add(position, moveDir);
	position = Physics::ClampToArena(position, halfArena);
}

void Player::Draw()const{
	DrawCubeV(position, {size, size, size}, BLUE);
	DrawCubeWiresV(position, {size, size, size}, ColorAlpha(BLUE, 0.4F));
}

void Player::Respawn(){
	position = {0.0F, 0.0F, 0.0F};
	fmt::println("[Player] respawned. Lives remaining: {}", lives);
}
