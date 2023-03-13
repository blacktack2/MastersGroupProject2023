#pragma once

namespace NCL {
	enum class CollisionLayer {
		Default = 0,
		Player,
		Enemy,
		PlayerProj,
		EnemyProj,
		PlayerTrig,
		PaintAble,
		MAX_LAYER,


		InteractionDefault_    = 1 << (int)Default | 1 << (int)Player | 1 << (int)Enemy                        | 1 << (int)EnemyProj | 1 << (int)PlayerTrig | 1 << (int)PaintAble,
		InteractionPlayer_     = 1 << (int)Default |                    1 << (int)Enemy                        | 1 << (int)EnemyProj |                        1 << (int)PaintAble,
		InteractionEnemy_      = 1 << (int)Default | 1 << (int)Player | 1 << (int)Enemy 																	| 1 << (int)PaintAble,
		InteractionPlayerProj_ =									    1 << (int)Enemy																	    | 1 << (int)PaintAble,
		InteractionEnemyProj_  =					 1 << (int)Player |                                                                                       1 << (int)PaintAble,
		InteractionPlayerTrig_ = 1 << (int)Default                                                                                                                               ,
		InteractionPaintable_  =                     1 << (int)Player | 1 << (int)Enemy | 1 << (int)PlayerProj | 1 << (int)EnemyProj                                             ,
	};

	const int layerInteractions[(size_t)CollisionLayer::MAX_LAYER] = {
		(int)CollisionLayer::InteractionDefault_,
		(int)CollisionLayer::InteractionPlayer_,
		(int)CollisionLayer::InteractionEnemy_,
		(int)CollisionLayer::InteractionPlayerProj_,
		(int)CollisionLayer::InteractionEnemyProj_,
		(int)CollisionLayer::InteractionPlayerTrig_,
		(int)CollisionLayer::InteractionPaintable_,
	};

	static inline bool DoCollisionLayersOverlap(CollisionLayer a, CollisionLayer b) {
		return layerInteractions[(int)a] & (1 << (int)b);
	}
}