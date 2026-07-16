#include "Game.h"
#include "Physics.h"
#include <fmt/base.h>
#include <raylib.h>
#include <raymath.h>

Game::Game(float arenaSize) : halfArena(arenaSize / 2.0F){
	enemyMgr.SpawnAll(halfArena, 8);
	fmt::println("[Game] Arena {}*{} - ready", arenaSize, arenaSize);
}

void Game::Update(float dt, Vector3 moveDir, const Camera3D& camera, bool shootPressed){
	//Player
	player.Update(dt, moveDir,  halfArena);


	//Update Enemies
	enemyMgr.Update(dt,  halfArena);

	//Update Bullets (Add this line!)
	bulletPool.Update(dt,  halfArena);

	//shoot
	shootCooldown -= dt;
	if(shootPressed && shootCooldown <= 0.0F){
		Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
		forward.y = 0.0F;
		Vector3 origin = Vector3Add(player.position, Vector3Scale(forward, player.size + 0.5F));
		bulletPool.Spawn(origin, forward, 40.0F);
		shootCooldown = 0.18F; // fir rate limiter
	}

	//Collisions: bullet * enemy
	for(auto& b:bulletPool.bullets){
		if(!b.active) continue;
		for(auto& e:enemyMgr.enemies){
			if(!e.active) continue;
			if(Physics::bulletHitEnemy(b.position, b.size, e.position, e.size)){
				b.active = false;
				e.active = false;
				score++;
				fmt::println("[Score] + 1 total={}", score);
			}
		}
	}

	//Collisions: player * enemy
	for(auto& e:enemyMgr.enemies){
		if(!e.active) continue;
		if(Physics::playerHitEnemy(player.position, player.size, e.position, e.size)){
			player.lives--;
			fmt::println("[Player] hit! lives={}", player.lives);
			player.Respawn();
			//Respawn the enemy on the other side
			//
			float x = (float)GetRandomValue(-(int)halfArena + 2, (int)halfArena - 2);
			float z = (float)GetRandomValue(-(int)halfArena + 2, (int)halfArena - 2);
			e.position = {x, 0.0F, z};
			e.velocity = {(float)GetRandomValue(-5, 5), 0, (float)GetRandomValue(-5, 5)};
			break; // only one hit per frame
		}
	}

	//Respawn killed enemies after a delay (simple: every 3 seconds)
	static float respawnTimer = 0.0F;
	respawnTimer += dt;
	if(respawnTimer > 3.0F){
		respawnTimer = 0.0F;
		for(auto& e:enemyMgr.enemies){
			if(!e.active){
				e.active = true;
				float x = (float)GetRandomValue(-(int)halfArena + 2, (int)halfArena - 2);
				float z = (float)GetRandomValue(-(int)halfArena + 2, (int)halfArena - 2);
				e.position = {x, 0.5F, z};
				break;
			}
		}
	}
}

void Game::Draw() const {
	player.Draw();
	bulletPool.Draw();
	enemyMgr.Draw();

	//Arena wireframe
	DrawCubeWiresV({0,0,0}, {halfArena* 2, halfArena * 2, halfArena*2},
		ColorAlpha(SKYBLUE, 0.25F) );
}
