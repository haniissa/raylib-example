#pragma once
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include <raylib.h>

class Game{
	private:
		float halfArena;
		Player player;
		BulletPool bulletPool;
		EnemyManager enemyMgr;
		int score = 0;
		float shootCooldown = 0.0F;
	public:
		explicit Game(float arenaSize);
		void Update(float dt, Vector3 moveDir, const Camera3D& camera, bool shootPressed);
		void Draw() const;

		Player&  GetPlayer() {return player;}
		const Player& GetPlayer() const  {return player;}
		int   GetScore() const {return score;}
		void spawnEnemies(int count){enemyMgr.SpawnAll(halfArena, count);}
};
