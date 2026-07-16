#pragma once
#include <raylib.h>
#include <raymath.h>

namespace Physics{
	inline BoundingBox cubeAABB(Vector3 center, float halfSize){
		return BoundingBox{
			Vector3Subtract(center, {halfSize, halfSize, halfSize}),
			Vector3Add(center, {halfSize, halfSize, halfSize})
		};
	}

	inline bool bulletHitEnemy(Vector3 bPos, float bSize, Vector3 ePos, float eSize) {
		BoundingBox b = cubeAABB(bPos, bSize);
		BoundingBox e = cubeAABB(ePos, eSize);
		return CheckCollisionBoxes(b, e);
	}


	inline bool playerHitEnemy(Vector3 pPos, float pSize, Vector3 ePos, float eSize) {
		BoundingBox p = cubeAABB(pPos, pSize);
		BoundingBox e = cubeAABB(ePos, eSize);
		return CheckCollisionBoxes(p, e);
	}

	inline bool IsOutOfBounds(Vector3 pos, float arenaHalf){
		return (
			fabs(pos.x) > arenaHalf or
			fabs(pos.y) > arenaHalf or
			fabs(pos.z) > arenaHalf
		);
	}

	inline Vector3 ClampToArena(Vector3 pos, float halfArena){
		pos.x = Clamp(pos.x, -halfArena, halfArena);
		pos.y = Clamp(pos.y, -halfArena, halfArena);
		pos.z = Clamp(pos.z, -halfArena, halfArena);
		return pos;
	}
} // namespace Physics
