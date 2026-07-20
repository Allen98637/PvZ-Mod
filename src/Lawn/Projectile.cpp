/*
 * Copyright (C) 2026 Zhou Qiankang <wszqkzqk@qq.com>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * This file is part of PvZ-Portable.
 *
 * PvZ-Portable is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PvZ-Portable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with PvZ-Portable. If not, see <https://www.gnu.org/licenses/>.
 */

#include "Board.h"
#include "Plant.h"
#include "Zombie.h"
#include "Cutscene.h"
#include "Projectile.h"
#include "../LawnApp.h"
#include "../Resources.h"
#include "../GameConstants.h"
#include "../Sexy.TodLib/TodFoley.h"
#include "../Sexy.TodLib/TodDebug.h"
#include "../Sexy.TodLib/Reanimator.h"
#include "../Sexy.TodLib/Attachment.h"
#include "Widget/AchievementsScreen.h"

constinit const ProjectileDefinition gProjectileDefinition[] = {
	{ .mProjectileType = ProjectileType::PROJECTILE_PEA, .mImageRow = 0, .mDamage = 20 },
	{ .mProjectileType = ProjectileType::PROJECTILE_SNOWPEA, .mImageRow = 0, .mDamage = 20 },
	{ .mProjectileType = ProjectileType::PROJECTILE_CABBAGE, .mImageRow = 0, .mDamage = 40 },
	{ .mProjectileType = ProjectileType::PROJECTILE_MELON, .mImageRow = 0, .mDamage = 80 },
	{ .mProjectileType = ProjectileType::PROJECTILE_PUFF, .mImageRow = 0, .mDamage = 20 },
	{ .mProjectileType = ProjectileType::PROJECTILE_WINTERMELON, .mImageRow = 0, .mDamage = 80 },
	{ .mProjectileType = ProjectileType::PROJECTILE_FIREBALL, .mImageRow = 0, .mDamage = 40 },
	{ .mProjectileType = ProjectileType::PROJECTILE_STAR, .mImageRow = 0, .mDamage = 20 },
	{ .mProjectileType = ProjectileType::PROJECTILE_SPIKE, .mImageRow = 0, .mDamage = 20 },
	{ .mProjectileType = ProjectileType::PROJECTILE_BASKETBALL, .mImageRow = 0, .mDamage = 75 },
	{ .mProjectileType = ProjectileType::PROJECTILE_KERNEL, .mImageRow = 0, .mDamage = 20 },
	{ .mProjectileType = ProjectileType::PROJECTILE_COBBIG, .mImageRow = 0, .mDamage = 300 },
	{ .mProjectileType = ProjectileType::PROJECTILE_BUTTER, .mImageRow = 0, .mDamage = 40 },
	{ .mProjectileType = ProjectileType::PROJECTILE_ZOMBIE_PEA, .mImageRow = 0, .mDamage = 20 },
	{ .mProjectileType = ProjectileType::PROJECTILE_ZOMBIE_CABBAGE, .mImageRow = 0, .mDamage = 40  },
	{ .mProjectileType = ProjectileType::PROJECTILE_ZOMBIE_MELON, .mImageRow = 0, .mDamage = 80  },
	{ .mProjectileType = ProjectileType::PROJECTILE_ZOMBIE_SNOWPEA, .mImageRow = 0, .mDamage = 20  },
	{ .mProjectileType = ProjectileType::PROJECTILE_ZOMBIE_WINTERMELON, .mImageRow = 0, .mDamage = 80  }
};

Projectile::Projectile()
{
}

Projectile::~Projectile()
{
	AttachmentDie(mAttachmentID);
}

void Projectile::ProjectileInitialize(int theX, int theY, int theRenderOrder, int theRow, ProjectileType theProjectileType)
{
	int aGridX = mBoard->PixelToGridXKeepOnBoard(theX, theY);
	mProjectileType = theProjectileType;
	mPosX = theX;
	mPosY = theY;
	mPosZ = 0.0f;
	mVelX = 0.0f;
	mVelY = 0.0f;
	mVelZ = 0.0f;
	mAccZ = 0.0f;
	mShadowY = mBoard->GridToPixelY(aGridX, theRow) + 67.0f;
	mHitTorchwoodGridX = -1;
	mMotionType = ProjectileMotion::MOTION_STRAIGHT;
	mFrame = 0;
	mNumFrames = 1;
	mRow = theRow;
	mCobTargetX = 0.0f;
	mDamageRangeFlags = 0;
	mDead = false;
	mAttachmentID = AttachmentID::ATTACHMENTID_NULL;
	mCobTargetRow = 0;
	mTargetID = {false, ZombieID::ZOMBIEID_NULL};
	mOnHighGround = mBoard->mGridSquareType[aGridX][theRow] == GridSquareType::GRIDSQUARE_HIGH_GROUND;
	if (mBoard->StageHasRoof())
	{
		mShadowY -= 12.0f;
	}
	mRenderOrder = theRenderOrder;
	mRotation = 0.0f;
	mRotationSpeed = 0.0f;
	mWidth = 40;
	mHeight = 40;
	mPlantSide = !IsZombieProjectile();
	mProjectileAge = 0;
	mClickBackoffCounter = 0;
	mAnimTicksPerFrame = 0;

	InitSpeed();

	switch (mProjectileType)
	{
	case ProjectileType::PROJECTILE_CABBAGE:
	case ProjectileType::PROJECTILE_BUTTER:
		mRotation = -7 * PI / 25;  // DEG_TO_RAD(-50.4f);
		mRotationSpeed = RandRangeFloat(-0.08f, -0.02f);
		mUmbrellaParticle = PARTICLE_UMBRELLA_REFLECT_CABBAGE;
		if(mProjectileType == PROJECTILE_BUTTER) mUmbrellaParticle = PARTICLE_UMBRELLA_REFLECT_BUTTER;
		break;
	case ProjectileType::PROJECTILE_ZOMBIE_CABBAGE:
		mRotation = 7 * PI / 25;  // DEG_TO_RAD(-50.4f);
		mRotationSpeed = RandRangeFloat(0.08f, 0.02f);
		mUmbrellaParticle = PARTICLE_UMBRELLA_REFLECT_CABBAGE2;
		break;
	case ProjectileType::PROJECTILE_MELON:
	case ProjectileType::PROJECTILE_WINTERMELON:
		mRotation = -2 * PI / 5;  // DEG_TO_RAD(-72.0f);
		mRotationSpeed = RandRangeFloat(-0.08f, -0.02f);
		mUmbrellaParticle = PARTICLE_UMBRELLA_REFLECT_MELON;
		if(mProjectileType == PROJECTILE_WINTERMELON) mUmbrellaParticle = PARTICLE_UMBRELLA_REFLECT_WINTERMELON;
		break;
	case ProjectileType::PROJECTILE_ZOMBIE_MELON:
		mRotation = 2 * PI / 5;  // DEG_TO_RAD(-72.0f);
		mRotationSpeed = RandRangeFloat(0.08f, 0.02f);
		mUmbrellaParticle = PARTICLE_UMBRELLA_REFLECT_MELON2;
		break;
	case ProjectileType::PROJECTILE_ZOMBIE_WINTERMELON:
		mRotation = 2 * PI / 5;  // DEG_TO_RAD(-72.0f);
		mRotationSpeed = RandRangeFloat(0.08f, 0.02f);
		mUmbrellaParticle = PARTICLE_UMBRELLA_REFLECT_WINTERMELON2;
		break;
	case ProjectileType::PROJECTILE_KERNEL:
		mRotation = 0.0f;
		mRotationSpeed = RandRangeFloat(-0.2f, -0.08f);
		mUmbrellaParticle = PARTICLE_UMBRELLA_REFLECT_KERNEL;
		break;
	case ProjectileType::PROJECTILE_SNOWPEA:
	{
		TodParticleSystem* aParticle = mApp->AddTodParticle(mPosX + 8.0f, mPosY + 13.0f, 400000, ParticleEffect::PARTICLE_SNOWPEA_TRAIL);
		AttachParticle(mAttachmentID, aParticle, 8.0f, 13.0f);
		break;
	}
	case ProjectileType::PROJECTILE_ZOMBIE_SNOWPEA:
	{
		TodParticleSystem* aParticle = mApp->AddTodParticle(mPosX - 8.0f, mPosY + 13.0f, 400000, ParticleEffect::PARTICLE_SNOWPEA_TRAIL);
		AttachParticle(mAttachmentID, aParticle, -8.0f, 13.0f);
		break;
	}
	case ProjectileType::PROJECTILE_FIREBALL:
		TOD_ASSERT(false);
		break;
	case ProjectileType::PROJECTILE_COBBIG:
		mWidth = IMAGE_REANIM_COBCANNON_COB->GetWidth();
		mHeight = IMAGE_REANIM_COBCANNON_COB->GetHeight();
		mRotation = PI / 2;
		break;
	case ProjectileType::PROJECTILE_PUFF:
	{
		TodParticleSystem* aParticle = mApp->AddTodParticle(mPosX + 13.0f, mPosY + 13.0f, 400000, ParticleEffect::PARTICLE_PUFFSHROOM_TRAIL);
		AttachParticle(mAttachmentID, aParticle, 13.0f, 13.0f);
		break;
	}
	case ProjectileType::PROJECTILE_BASKETBALL:
		mRotation = RandRangeFloat(0.0f, 2 * PI);
		mRotationSpeed = RandRangeFloat(0.05f, 0.1f);
		mUmbrellaParticle = PARTICLE_UMBRELLA_REFLECT;
		break;
	case ProjectileType::PROJECTILE_STAR:
		mShadowY += 15.0f;
		mRotationSpeed = RandRangeFloat(0.05f, 0.1f);
		if (Rand(2) == 0)
		{
			mRotationSpeed = -mRotationSpeed;
		}
		break;
	default:
		break;
	}

	mAnimCounter = 0;
	mX = static_cast<int>(mPosX);
	mY = static_cast<int>(mPosY);
}

bool Projectile::PeaAboutToHitTorchwood()
{
	if (mMotionType != ProjectileMotion::MOTION_STRAIGHT)
		return false;

	if (mProjectileType != ProjectileType::PROJECTILE_PEA && mProjectileType != ProjectileType::PROJECTILE_SNOWPEA)
		return false;

	Plant* aPlant = nullptr;
	while (mBoard->IteratePlants(aPlant))
	{
		if (aPlant->mSeedType == SeedType::SEED_TORCHWOOD && aPlant->mRow == mRow && !aPlant->NotOnGround() && mHitTorchwoodGridX != aPlant->mPlantCol)
		{
			Rect aPlantAttackRect = aPlant->GetPlantAttackRect(PlantWeapon::WEAPON_PRIMARY);
			Rect aProjectileRect = GetProjectileRect();
			aProjectileRect.mX += 40;

			if (GetRectOverlap(aPlantAttackRect, aProjectileRect) > 10)
			{
				return true;
			}
		}
	}

	return false;
}

PlantOrZombie Projectile::FindCollisionTarget(bool doPlant)
{
	PlantOrZombie aBestEnemy;
	if (PeaAboutToHitTorchwood())  // “卡火炬”的原理，这段代码在两版内测版中均不存在
		return aBestEnemy;

	Rect aProjectileRect = GetProjectileRect();
	int aMinX = 0;
	int jjg = mVelX < 0?-1:1;

	Zombie* aZombie = nullptr;
	while (mBoard->IterateZombies(aZombie))
	{
		if(mPlantSide == aZombie->mMindControlled) continue;
		if ((aZombie->mZombieType == ZombieType::ZOMBIE_BOSS || aZombie->mRow == mRow) && aZombie->EffectedByDamage(static_cast<unsigned int>(mDamageRangeFlags)))
		{
			if (aZombie->mZombiePhase == ZombiePhase::PHASE_SNORKEL_WALKING_IN_POOL && mPosZ >= 45.0f)
			{
				continue;
			}

			if (mProjectileType == ProjectileType::PROJECTILE_STAR && mProjectileAge < 25 && mVelX >= 0.0f && aZombie->mZombieType == ZombieType::ZOMBIE_DIGGER)
			{
				continue;
			}

			Rect aZombieRect = aZombie->GetZombieRect();
			if (GetRectOverlap(aProjectileRect, aZombieRect) > 0)
			{
				if (aBestEnemy == nullptr || (aZombie->mX - aMinX) / jjg < 0)
				{
					aBestEnemy = aZombie;
					aMinX = aZombie->mX;
				}
			}
		}
	}

	if(!doPlant) return aBestEnemy;

	Plant* aPlant = nullptr;
	while (mBoard->IteratePlants(aPlant))
	{
		if(mPlantSide != aPlant->mMindControlled) continue;
		if (aPlant->mRow != mRow)
			continue;

		if (mMotionType != ProjectileMotion::MOTION_LOBBED)
		{
			if (aPlant->mSeedType == SeedType::SEED_PUFFSHROOM ||
				aPlant->mSeedType == SeedType::SEED_SUNSHROOM ||
				aPlant->mSeedType == SeedType::SEED_POTATOMINE ||
				aPlant->mSeedType == SeedType::SEED_SPIKEWEED ||
				aPlant->mSeedType == SeedType::SEED_SPIKEROCK ||
				aPlant->mSeedType == SeedType::SEED_LILYPAD)  // 僵尸豌豆不能击中低矮植物
				continue;
		}

		Rect aPlantRect = aPlant->GetPlantRect();
		if (GetRectOverlap(aProjectileRect, aPlantRect) > 8)
		{
			if(aBestEnemy == nullptr || (mBoard->GridToPixelX(aPlant->mPlantCol, aPlant->mRow) - aMinX) / jjg < 0){
				if (mMotionType == ProjectileMotion::MOTION_LOBBED)
				{
					aBestEnemy = mBoard->GetTopPlantAt(aPlant->mPlantCol, aPlant->mRow, PlantPriority::TOPPLANT_CATAPULT_ORDER);
				}
				else
				{
					aBestEnemy = mBoard->GetTopPlantAt(aPlant->mPlantCol, aPlant->mRow, PlantPriority::TOPPLANT_EATING_ORDER);
				}
			}
			break;
		}
	}

	return aBestEnemy;
}

void Projectile::CheckForCollision()
{
	if (mMotionType == ProjectileMotion::MOTION_PUFF && mProjectileAge >= 75)
	{
		Die();
		return;
	}

	if (mPosX > WIDE_BOARD_WIDTH || mPosX + mWidth < 0.0f)
	{
		Die();
		return;
	}

	if (mMotionType == ProjectileMotion::MOTION_HOMING)
	{
		PlantOrZombie aEnemy = mBoard->POZTryToGet(mTargetID);
		if (aEnemy.mZombie && aEnemy.mZombie->EffectedByDamage(static_cast<unsigned int>(mDamageRangeFlags)))
		{
			Rect aProjectileRect = GetProjectileRect();
			Rect aZombieRect = aEnemy.mZombie->GetZombieRect();
			if (GetRectOverlap(aProjectileRect, aZombieRect) >= 0 && mPosY > aZombieRect.mY && mPosY < aZombieRect.mY + aZombieRect.mHeight)
			{
				DoImpact(aEnemy);
			}
		}
		else if(aEnemy.mPlant){
			Rect aProjectileRect = GetProjectileRect();
			Rect aPlantRect = aEnemy.mPlant->GetPlantRect();
			if (aProjectileRect.Intersects(aPlantRect))
			{
				DoImpact(aEnemy);
			}
		}
		return;
	}

	if (mProjectileType == ProjectileType::PROJECTILE_STAR && (mPosY > 600.0f || mPosY < 0.0f))
	{
		Die();
		return;
	}

	if (/*(mProjectileType == ProjectileType::PROJECTILE_PEA || mProjectileType == ProjectileType::PROJECTILE_STAR) &&*/ mShadowY - mPosY > 90.0f)
	{
		return;
	}

	if (mMotionType == ProjectileMotion::MOTION_FLOAT_OVER)
	{
		return;
	}

	PlantOrZombie aEnemy = FindCollisionTarget(true);
	if (aEnemy)
	{
		if (aEnemy.mZombie && aEnemy.mZombie->mOnHighGround && CantHitHighGround())
		{
			return;
		}

		DoImpact(aEnemy);
	}
}

bool Projectile::CantHitHighGround()
{
	if (mMotionType == ProjectileMotion::MOTION_BACKWARDS || mMotionType == ProjectileMotion::MOTION_HOMING)
		return false;

	return (
		mProjectileType == ProjectileType::PROJECTILE_PEA ||
		mProjectileType == ProjectileType::PROJECTILE_SNOWPEA ||
		mProjectileType == ProjectileType::PROJECTILE_STAR ||
		mProjectileType == ProjectileType::PROJECTILE_PUFF ||
		mProjectileType == ProjectileType::PROJECTILE_FIREBALL
		) && !mOnHighGround;
}

void Projectile::CheckForHighGround()
{
	PlantOrZombie nuul;
	float aShadowDelta = mShadowY - mPosY;

	if (mProjectileType == ProjectileType::PROJECTILE_PEA ||
		mProjectileType == ProjectileType::PROJECTILE_SNOWPEA ||
		mProjectileType == ProjectileType::PROJECTILE_FIREBALL ||
		mProjectileType == ProjectileType::PROJECTILE_SPIKE ||
		mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		if (aShadowDelta < 28.0f)
		{
			DoImpact(nuul);
			return;
		}
	}

	if (mProjectileType == ProjectileType::PROJECTILE_PUFF && aShadowDelta < 0.0f)
	{
		DoImpact(nuul);
		return;
	}

	if (mProjectileType == ProjectileType::PROJECTILE_STAR && aShadowDelta < 23.0f)
	{
		DoImpact(nuul);
		return;
	}

	if (CantHitHighGround())
	{
		int aGridX = mBoard->PixelToGridXKeepOnBoard(mPosX + 30, mPosY);
		if (mBoard->mGridSquareType[aGridX][mRow] == GridSquareType::GRIDSQUARE_HIGH_GROUND)
		{
			DoImpact(nuul);
		}
	}
}

bool Projectile::IsSplashDamage(PlantOrZombie theEnemy)
{
	if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL && theEnemy.mZombie && theEnemy.mZombie->IsFireResistant())
		return false;

	return
		mProjectileType == ProjectileType::PROJECTILE_MELON ||
		mProjectileType == ProjectileType::PROJECTILE_WINTERMELON ||
		mProjectileType == ProjectileType::PROJECTILE_FIREBALL ||
		mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_MELON ||
		mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_WINTERMELON;;
}

unsigned int Projectile::GetDamageFlags(PlantOrZombie theEnemy)
{
	unsigned int aDamageFlags = 0U;

	if(!mPlantSide) SetBit(aDamageFlags, static_cast<int>(DamageRangeFlags::DAMAGES_ONLY_MINDCONTROLLED), true);
	if (IsSplashDamage(theEnemy))
	{
		SetBit(aDamageFlags, static_cast<int>(DamageFlags::DAMAGE_BYPASSES_SHIELD), true);
	}
	else if (mMotionType == ProjectileMotion::MOTION_LOBBED || mMotionType == ProjectileMotion::MOTION_BACKWARDS)
	{
		SetBit(aDamageFlags, static_cast<int>(DamageFlags::DAMAGE_BYPASSES_SHIELD), true);
	}
	else if (mMotionType == ProjectileMotion::MOTION_STAR && mVelX < 1e-6f)
	{
		SetBit(aDamageFlags, static_cast<int>(DamageFlags::DAMAGE_BYPASSES_SHIELD), true);
	}
	else if (mMotionType == ProjectileMotion::MOTION_HOMING)
	{
		SetBit(aDamageFlags, static_cast<int>(DamageFlags::DAMAGE_BYPASSES_SHIELD), mVelX < 1e-6f);
	}

	if (mProjectileType == ProjectileType::PROJECTILE_SNOWPEA || mProjectileType == ProjectileType::PROJECTILE_WINTERMELON ||
		mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_SNOWPEA || mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_WINTERMELON
	)
	{
		SetBit(aDamageFlags, static_cast<int>(DamageFlags::DAMAGE_FREEZE), true);
	}

	return aDamageFlags;
}

bool Projectile::IsHitBySplash(PlantOrZombie theEnemy)
{
	Rect aProjectileRect = GetProjectileRect();
	if(theEnemy.mZombie){
		if(mPlantSide == theEnemy.mZombie->mMindControlled) return false;
		if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
		{
			if(mVelX < 0) aProjectileRect.mX -= 100 - aProjectileRect.mWidth;
			aProjectileRect.mWidth = 100;
		}

		int aRowDeviation = theEnemy.mZombie->mRow - mRow;
		Rect aZombieRect = theEnemy.mZombie->GetZombieRect();
		if (theEnemy.mZombie->IsFireResistant() && mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
		{
			return false;
		}

		if (theEnemy.mZombie->mZombieType == ZombieType::ZOMBIE_BOSS)
		{
			aRowDeviation = 0;
		}
		if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
		{
			if (aRowDeviation != 0)
			{
				return false;
			}
		}
		else if (aRowDeviation > 1 || aRowDeviation < -1)
		{
			return false;
		}

		return theEnemy.mZombie->EffectedByDamage(static_cast<unsigned int>(mDamageRangeFlags)) && GetRectOverlap(aProjectileRect, aZombieRect) >= 0;
	}
	else if(theEnemy.mPlant){
		if(mPlantSide != theEnemy.mPlant->mMindControlled) return false;
		int aRowDeviation = theEnemy.mPlant->mRow - mRow;
		Rect aPlantRect = theEnemy.mPlant->GetPlantRect();
		if (aRowDeviation > 1 || aRowDeviation < -1)
		{
			return false;
		}

		return GetRectOverlap(aProjectileRect, aPlantRect) >= 0;
	}
	return false;
}

void Projectile::DoSplashDamage(PlantOrZombie theEnemy)
{
	const ProjectileDefinition& aProjectileDef = GetProjectileDef();

	int aEnemiesGetSplashed = 0;
	Zombie* aZombie = nullptr;
	while (mBoard->IterateZombies(aZombie))
	{
		PlantOrZombie daZ(aZombie);
		if (mPlantSide != aZombie->mMindControlled && aZombie != theEnemy && IsHitBySplash(daZ))
		{
			aEnemiesGetSplashed++;
		}
	}
	Plant* aPlant = nullptr;
	while (mBoard->IteratePlants(aPlant))
	{
		PlantOrZombie daP(aPlant);
		if (mPlantSide == aPlant->mMindControlled && aPlant != theEnemy && IsHitBySplash(daP))
		{
			aEnemiesGetSplashed++;
		}
	}

	int aOriginalDamage = aProjectileDef.mDamage;
	int aSplashDamage = aProjectileDef.mDamage / 3;
	int aMaxSplashDamageAmount = aOriginalDamage * 7;
	if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
	{
		aMaxSplashDamageAmount = aOriginalDamage;
	}
	int aSplashDamageAmount = aSplashDamage * aEnemiesGetSplashed;
	if (aSplashDamageAmount > aMaxSplashDamageAmount)
	{
		//aSplashDamage *= aMaxSplashDamageAmount / aSplashDamage;
		aSplashDamage = aOriginalDamage * aMaxSplashDamageAmount / (aSplashDamageAmount * 3);
		aSplashDamage = std::max(aSplashDamage, 1);
	}

	aZombie = nullptr;
	while (mBoard->IterateZombies(aZombie))
	{
		PlantOrZombie daP(aZombie);
		if (IsHitBySplash(daP))
		{
			unsigned int aDamageFlags = GetDamageFlags(daP);
			if (aZombie == theEnemy)
			{
				aZombie->TakeDamage(aOriginalDamage, aDamageFlags);
			}
			else
			{
				aZombie->TakeDamage(aSplashDamage, aDamageFlags);
			}
		}
	}
	aPlant = nullptr;
	while (mBoard->IteratePlants(aPlant))
	{
		PlantOrZombie daP(aPlant);
		if (IsHitBySplash(aPlant))
		{
			unsigned int aDamageFlags = GetDamageFlags(theEnemy);
			if (aPlant == theEnemy)
			{
				aPlant->mPlantHealth -= aOriginalDamage;
			}
			else
			{
				aPlant->mPlantHealth -= aSplashDamage;
			}
			aPlant->mEatenFlashCountdown = std::max(aPlant->mEatenFlashCountdown, 25);
			if(TestBit(aDamageFlags, static_cast<int>(DamageFlags::DAMAGE_FREEZE))) aPlant->ApplyChill(false);
		}
	}
	if (theEnemy.mZombie && theEnemy.mZombie->mShieldType != ShieldType::SHIELDTYPE_NONE)
    {
        theEnemy.mZombie->TakeShieldDamage(aSplashDamage, DamageFlags::DAMAGE_HITS_SHIELD_AND_BODY);
    }
}

// GOTY @Patoke: 0x471B41
void Projectile::UpdateLobMotion()
{
	if (mProjectileType == ProjectileType::PROJECTILE_COBBIG && mPosZ < -700.0f)
	{
		mVelZ = 8.0f;
		mRow = mCobTargetRow;
		mPosX = mCobTargetX;
		int aCobTargetCol = mBoard->PixelToGridXKeepOnBoard(mCobTargetX, 0);
		mPosY = mBoard->GridToPixelY(aCobTargetCol, mCobTargetRow);
		mShadowY = mPosY + 67.0f;
		mRotation = -PI / 2;
	}

	mVelZ += mAccZ;
	if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HIGH_GRAVITY)
	{
		mVelZ += mAccZ;
	}
	mPosX += mVelX;
	mPosY += mVelY;
	mPosZ += mVelZ;

	bool isRising = mVelZ < 0.0f;
	if (isRising && (mProjectileType == ProjectileType::PROJECTILE_BASKETBALL || mProjectileType == ProjectileType::PROJECTILE_COBBIG))
	{
		return;
	}
	float aMinCollisionZ = 0.0f;
	if (mProjectileAge > 20)
	{
		if (isRising)
		{
			return;
		}

		if (mProjectileType == ProjectileType::PROJECTILE_BUTTER)
		{
			aMinCollisionZ = -32.0f;
		}
		else if (mProjectileType == ProjectileType::PROJECTILE_BASKETBALL || mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_CABBAGE)
		{
			aMinCollisionZ = -30.0f;
		}
		else if(mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_MELON || mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_WINTERMELON){
			aMinCollisionZ = -30.0f;
		}
		else if (mProjectileType == ProjectileType::PROJECTILE_MELON || mProjectileType == ProjectileType::PROJECTILE_WINTERMELON)
		{
			aMinCollisionZ = -35.0f;
		}
		else if (mProjectileType == ProjectileType::PROJECTILE_CABBAGE || mProjectileType == ProjectileType::PROJECTILE_KERNEL)
		{
			aMinCollisionZ = -30.0f;
		}
		else if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
		{
			aMinCollisionZ = -60.0f;
		}
		if (mBoard->mPlantRow[mRow] == PlantRowType::PLANTROW_POOL)
		{
			aMinCollisionZ += 40.0f;
		}

		if (mPosZ <= aMinCollisionZ)
		{
			return;
		}
	}

	PlantOrZombie aEnemy;
	aEnemy = FindCollisionTarget(mProjectileAge <= 20 || mPosZ > aMinCollisionZ + 90);

	float aGroundZ = 80.0f;
	if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		aGroundZ = -40.0f;
	}
	bool hitGround = mPosZ > aGroundZ;
	if (!aEnemy && !hitGround)
	{
		return;
	}

	if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		// @Patoke: implemented
		int aBeforeGargantuarCount = mBoard->GetLiveGargantuarCount();
		mBoard->KillAllZombiesInRadius(mRow, mPosX + 80, mPosY + 40, 115, 1, true, mDamageRangeFlags);
		int aAfterGargantuarCount = mBoard->GetLiveGargantuarCount();
		mBoard->mGargantuarsKillsByCornCob += aBeforeGargantuarCount - aAfterGargantuarCount;
		if (mBoard->mGargantuarsKillsByCornCob >= 2)
			ReportAchievement::GiveAchievement(mApp, PopcornParty, true);

		DoImpact(PlantOrZombie());
	}
	else if(aEnemy)
	{
		Rect theRect = GetProjectileRect();
		Zombie* aUmbrellaZombie = mBoard->FindUmbrellaZombie(theRect, mRow, !mPlantSide);
		if (aUmbrellaZombie)
		{
			if (aUmbrellaZombie->mZombotomyState == PlantState::STATE_UMBRELLA_REFLECTING)
			{
				mApp->PlayFoley(FoleyType::FOLEY_SPLAT);
				int aRenderPosition = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_TOP, 0, 1);
				mApp->AddTodParticle(mPosX + 20.0f, mPosY + 20.0f, aRenderPosition, mUmbrellaParticle);
				Die();
			}
			else if (aUmbrellaZombie->mZombotomyState != PlantState::STATE_UMBRELLA_TRIGGERED)
			{
				mApp->PlayFoley(FoleyType::FOLEY_UMBRELLA);
				aUmbrellaZombie->DoSpecial();
			}
		}
		else{
			int aCol;
			if(aEnemy.mPlant) aCol = aEnemy.mPlant->mPlantCol;
			else aCol = mBoard->PixelToGridX((theRect.mX + theRect.mWidth) / 2, (theRect.mY + theRect.mHeight) / 2);
			Plant* aUmbrellaPlant = mBoard->FindUmbrellaPlant(aCol, mRow, mPlantSide);
			if (aUmbrellaPlant)
			{
				if (aUmbrellaPlant->mState == PlantState::STATE_UMBRELLA_REFLECTING)
				{
					mApp->PlayFoley(FoleyType::FOLEY_SPLAT);
					int aRenderPosition = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_TOP, 0, 1);
					mApp->AddTodParticle(mPosX + 20.0f, mPosY + 20.0f, aRenderPosition, mUmbrellaParticle);
					Die();
				}
				else if (aUmbrellaPlant->mState != PlantState::STATE_UMBRELLA_TRIGGERED)
				{
					mApp->PlayFoley(FoleyType::FOLEY_UMBRELLA);
					aUmbrellaPlant->DoSpecial();
				}
			}
			else{
				DoImpact(aEnemy);
			}
		}
	}
	else{
		DoImpact(aEnemy);
	}
}

void Projectile::UpdateNormalMotion()
{
	if (mMotionType == ProjectileMotion::MOTION_BACKWARDS)
	{
		mVelX = mPlantSide?-3.33f:3.33f;
		mPosX += mVelX;
	}
	else if (mMotionType == ProjectileMotion::MOTION_HOMING)
	{
		PlantOrZombie aZombie = mBoard->POZTryToGet(mTargetID);
		if(!aZombie || (aZombie.mZombie && !aZombie.mZombie->mHasHead)) aZombie = UpdateTargetZombie();
		if (aZombie.mZombie && aZombie.mZombie->EffectedByDamage(static_cast<unsigned int>(mDamageRangeFlags)))
		{
			Rect aZombieRect = aZombie.mZombie->GetZombieRect();
			mTargetX = aZombie.mZombie->ZombieTargetLeadX(0.0f);
			mTargetY = aZombieRect.mY + aZombieRect.mHeight / 2;
			SexyVector2 aTargetCenter(mTargetX, mTargetY);
			SexyVector2 aProjectileCenter(mPosX + mWidth / 2, mPosY + mHeight / 2);
			SexyVector2 aToTarget = (aTargetCenter - aProjectileCenter).Normalize();
			SexyVector2 aMotion(mVelX, mVelY);

			aMotion += aToTarget * (0.001f * mProjectileAge);
			aMotion = aMotion.Normalize();
			aMotion *= 2.0f;

			mVelX = aMotion.x;
			mVelY = aMotion.y;
			mRotation = -atan2(mVelY, mVelX);
		}
		else if (aZombie.mPlant)
		{
			Rect aZombieRect = aZombie.mPlant->GetPlantRect();
			mTargetX = aZombieRect.mX + aZombieRect.mWidth / 2;
			mTargetY = aZombieRect.mY + aZombieRect.mHeight / 2;
			SexyVector2 aTargetCenter(mTargetX, mTargetY);
			SexyVector2 aProjectileCenter(mPosX + mWidth / 2, mPosY + mHeight / 2);
			SexyVector2 aToTarget = (aTargetCenter - aProjectileCenter).Normalize();
			SexyVector2 aMotion(mVelX, mVelY);

			aMotion += aToTarget * (0.001f * mProjectileAge);
			aMotion = aMotion.Normalize();
			aMotion *= 2.0f;

			mVelX = aMotion.x;
			mVelY = aMotion.y;
			mRotation = -atan2(mVelY, mVelX);
		}

		mPosY += mVelY;
		mPosX += mVelX;
		mShadowY += mVelY;
		mRow = mBoard->PixelToGridYKeepOnBoard(mPosX, mPosY);
	}
	else if (mMotionType == ProjectileMotion::MOTION_STAR)
	{
		mPosY += mVelY;
		mPosX += mVelX;
		mShadowY += mVelY;

		if (mVelY != 0.0f)
		{
			mRow = mBoard->PixelToGridYKeepOnBoard(mPosX, mPosY);
		}
	}
	else if (mMotionType == ProjectileMotion::MOTION_BEE)
	{
		if (mProjectileAge < 60)
		{
			mPosY -= 0.5f;
		}
		mVelX = mPlantSide?3.33f:-3.33f;
		mPosX += mVelX;
	}
	else if (mMotionType == ProjectileMotion::MOTION_FLOAT_OVER)
	{
		if (mVelZ < 0.0f)
		{
			mVelZ += 0.002f;
			mVelZ = std::min(mVelZ, 0.0f);
			mPosY += mVelZ;
			mRotation = 0.3f - 0.7f * mVelZ * PI * 0.25f;
		}
		mVelX = mPlantSide?0.4f:-0.4f;
		mPosX += mVelX;
	}
	else if (mMotionType == ProjectileMotion::MOTION_BEE_BACKWARDS)
	{
		if (mProjectileAge < 60)
		{
			mPosY -= 0.5f;
		}
		mVelX = mPlantSide?-3.33f:3.33f;
		mPosX += mVelX;
	}
	else if (mMotionType == ProjectileMotion::MOTION_THREEPEATER)
	{
		mVelX = mPlantSide?3.33f:-3.33f;
		mPosX += mVelX;
		mPosY += mVelY;
		mVelY *= 0.97f;
		mShadowY += mVelY;
	}
	else
	{
		mVelX = mPlantSide?3.33f:-3.33f;
		mPosX += mVelX;
	}

	if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_HIGH_GRAVITY)
	{
		if (mMotionType == ProjectileMotion::MOTION_FLOAT_OVER)
		{
			mVelZ += 0.004f;
		}
		else
		{
			mVelZ += 0.2f;
		}

		mPosY += mVelZ;
	}

	CheckForCollision();
	CheckForHighGround();
}

void Projectile::InitSpeed(){
	if(mMotionType == MOTION_LOBBED) return;
	if (mMotionType == ProjectileMotion::MOTION_HOMING) return;
	if (mMotionType == ProjectileMotion::MOTION_STAR) return;
	if (mMotionType == ProjectileMotion::MOTION_BACKWARDS)
		mVelX = mPlantSide?-3.33f:3.33f;
	else if (mMotionType == ProjectileMotion::MOTION_BEE)
		mVelX = mPlantSide?3.33f:-3.33f;
	else if (mMotionType == ProjectileMotion::MOTION_FLOAT_OVER)
		mVelX = mPlantSide?0.4f:-0.4f;
	else if (mMotionType == ProjectileMotion::MOTION_BEE_BACKWARDS)
		mVelX = mPlantSide?-3.33f:3.33f;
	else if (mMotionType == ProjectileMotion::MOTION_THREEPEATER)
		mVelX = mPlantSide?3.33f:-3.33f;
	else
		mVelX = mPlantSide?3.33f:-3.33f;
}

PlantOrZombie Projectile::UpdateTargetZombie(){
	int aHighestWeight = 0;
    PlantOrZombie aBestEnemy;

    Zombie* aZombie = nullptr;
    while (mBoard->IterateZombies(aZombie)){
		if (!aZombie->EffectedByDamage(mDamageRangeFlags)) continue;
        Rect aZombieRect = aZombie->GetZombieRect();
        int aWeight = -Distance2D(mTargetX, mTargetY, aZombieRect.mX + aZombieRect.mWidth / 2, aZombieRect.mY + aZombieRect.mHeight / 2);
        if(!aZombie->mHasHead) aWeight -= 10000;
        if (aZombie->IsFlying())
        {
            aWeight += 10000;  // 优先攻击飞行单位
        }

        if (aBestEnemy == nullptr || aWeight > aHighestWeight)
        {
            aHighestWeight = aWeight;
            aBestEnemy = aZombie;
        }
	}
    Plant* aPlant = nullptr;
	while (mBoard->IteratePlants(aPlant)){
		if(aPlant->mMindControlled != mPlantSide) continue;
        Rect aPlantRect = aPlant->GetPlantRect();
        int aWeight = -Distance2D(mTargetX, mTargetY, aPlantRect.mX + aPlantRect.mWidth / 2, aPlantRect.mY + aPlantRect.mHeight / 2);

        if (aBestEnemy == nullptr || aWeight > aHighestWeight)
        {
            aHighestWeight = aWeight;
            aBestEnemy = aPlant;
        }
	}
	if(aBestEnemy != nullptr)
		mTargetID = mBoard->POZGetID(aBestEnemy);
	return aBestEnemy;
}

void Projectile::UpdateMotion()
{
	if (mAnimTicksPerFrame > 0)
	{
		mAnimCounter = (mAnimCounter + 1) % (mNumFrames * mAnimTicksPerFrame);
		mFrame = mAnimCounter / mAnimTicksPerFrame;
	}

	int aOldRow = mRow;
	float aOldY = mBoard->GetPosYBasedOnRow(mPosX, mRow);
	if (mMotionType == ProjectileMotion::MOTION_LOBBED)
	{
		UpdateLobMotion();
	}
	else
	{
		UpdateNormalMotion();
	}

	float aSlopeHeightChange = mBoard->GetPosYBasedOnRow(mPosX, aOldRow) - aOldY;
	if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		aSlopeHeightChange = 0.0f;  // Fix The Roof Offset Bug of Corn Cob
	}
	if (mMotionType == ProjectileMotion::MOTION_FLOAT_OVER)
	{
		mPosY += aSlopeHeightChange;
	}
	if (mMotionType == ProjectileMotion::MOTION_LOBBED)
	{
		mPosY += aSlopeHeightChange;
		mPosZ -= aSlopeHeightChange;
	}
	mShadowY += aSlopeHeightChange;
	mX = static_cast<int>(mPosX);
	mY = static_cast<int>(mPosY + mPosZ);
}

void Projectile::PlayImpactSound(PlantOrZombie theEnemy)
{
	bool aPlayHelmSound = true;
	bool aPlaySplatSound = true;
	if (mProjectileType == ProjectileType::PROJECTILE_KERNEL)
	{
		mApp->PlayFoley(FoleyType::FOLEY_KERNEL_SPLAT);
		aPlayHelmSound = false;
		aPlaySplatSound = false;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_BUTTER)
	{
		mApp->PlayFoley(FoleyType::FOLEY_BUTTER);
		aPlaySplatSound = false;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL && IsSplashDamage(theEnemy))
	{
		mApp->PlayFoley(FoleyType::FOLEY_IGNITE);
		aPlayHelmSound = false;
		aPlaySplatSound = false;
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_MELON || mProjectileType == ProjectileType::PROJECTILE_WINTERMELON ||
		mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_MELON || mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_WINTERMELON
	)
	{
		mApp->PlayFoley(FoleyType::FOLEY_MELONIMPACT);
		aPlaySplatSound = false;
	}

	if (aPlayHelmSound && theEnemy.mZombie)
	{
		if (theEnemy.mZombie->mHelmType == HELMTYPE_PAIL)
		{
			mApp->PlayFoley(FoleyType::FOLEY_SHIELD_HIT);
			aPlaySplatSound = false;
		}
		else if (theEnemy.mZombie->mHelmType == HELMTYPE_TRAFFIC_CONE || theEnemy.mZombie->mHelmType == HELMTYPE_DIGGER || 
			theEnemy.mZombie->mHelmType == HELMTYPE_FOOTBALL || theEnemy.mZombie->mHelmType == HELMTYPE_GIGA_FOOTBALL
		){
			mApp->PlayFoley(FoleyType::FOLEY_PLASTIC_HIT);
		}
	}

	if (aPlaySplatSound)
	{
		mApp->PlayFoley(FoleyType::FOLEY_SPLAT);
	}
}

void Projectile::DoImpact(PlantOrZombie theEnemy)
{
	PlayImpactSound(theEnemy);

	unsigned int aDamageFlags = GetDamageFlags(theEnemy);
	if (IsSplashDamage(theEnemy))
	{
		if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL && theEnemy.mZombie)
		{
			theEnemy.mZombie->RemoveColdEffects();
		}

		DoSplashDamage(theEnemy);
	}
	else if (theEnemy.mZombie)
	{
		theEnemy.mZombie->TakeDamage(GetProjectileDef().mDamage, aDamageFlags);
	}
	else if(theEnemy.mPlant)
	{
		theEnemy.mPlant->mPlantHealth -= GetProjectileDef().mDamage;
		theEnemy.mPlant->mEatenFlashCountdown = std::max(theEnemy.mPlant->mEatenFlashCountdown, 25);
		if(TestBit(aDamageFlags, static_cast<int>(DamageFlags::DAMAGE_FREEZE))){
			theEnemy.mPlant->ApplyChill(false);
		}
	}

	float aLastPosX = mPosX - mVelX;
	float aLastPosY = mPosY + mPosZ - mVelY - mVelZ;
	ParticleEffect aEffect = ParticleEffect::PARTICLE_NONE;
	float aSplatPosX = mPosX + 12.0f;
	float aSplatPosY = mPosY + 12.0f;
	switch (mProjectileType)
	{
	case ProjectileType::PROJECTILE_MELON:
		mApp->AddTodParticle(aLastPosX + 30.0f, aLastPosY + 30.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_MELONSPLASH);
		break;
	case ProjectileType::PROJECTILE_WINTERMELON:
		mApp->AddTodParticle(aLastPosX + 30.0f, aLastPosY + 30.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_WINTERMELON);
		break;
	case ProjectileType::PROJECTILE_COBBIG:
	{
		int aRenderOrder = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_GROUND, mCobTargetRow, 2);
		mApp->AddTodParticle(mPosX + 80.0f, mPosY + 40.0f, aRenderOrder, ParticleEffect::PARTICLE_BLASTMARK);
		mApp->AddTodParticle(mPosX + 80.0f, mPosY + 40.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_POPCORNSPLASH);
		mApp->PlaySample(SOUND_DOOMSHROOM);
		mBoard->ShakeBoard(3, -4);
		break;
	}
	case ProjectileType::PROJECTILE_PEA:
		aSplatPosX -= 15.0f;
		aEffect = ParticleEffect::PARTICLE_PEA_SPLAT;
		break;
	case ProjectileType::PROJECTILE_SNOWPEA:
		aSplatPosX -= 15.0f;
		aEffect = ParticleEffect::PARTICLE_SNOWPEA_SPLAT;
		break;
	case ProjectileType::PROJECTILE_FIREBALL:
	{
		if (IsSplashDamage(theEnemy))
		{
			Reanimation* aFireReanim = mApp->AddReanimation(mPosX + 38.0f, mPosY - 20.0f, mRenderOrder + 1, ReanimationType::REANIM_JALAPENO_FIRE);
			aFireReanim->mAnimTime = 0.25f;
			aFireReanim->mAnimRate = 24.0f;
			aFireReanim->OverrideScale(0.7f, 0.4f);
		}
		break;
	}
	case ProjectileType::PROJECTILE_STAR:
		aEffect = ParticleEffect::PARTICLE_STAR_SPLAT;
		break;
	case ProjectileType::PROJECTILE_PUFF:
		aSplatPosX -= 20.0f;
		aEffect = ParticleEffect::PARTICLE_PUFF_SPLAT;
		break;
	case ProjectileType::PROJECTILE_CABBAGE:
		aSplatPosX = aLastPosX - 38.0f;
		aSplatPosY = aLastPosY + 23.0f;
		aEffect = ParticleEffect::PARTICLE_CABBAGE_SPLAT;
		break;
	case ProjectileType::PROJECTILE_BUTTER:
		aSplatPosX = aLastPosX - 20.0f;
		aSplatPosY = aLastPosY + 63.0f;
		aEffect = ParticleEffect::PARTICLE_BUTTER_SPLAT;

		if (theEnemy.mZombie)
		{
			theEnemy.mZombie->ApplyButter();
		}
		break;
	case ProjectileType::PROJECTILE_ZOMBIE_PEA:
		mApp->AddTodParticle(mPosX - 3.0f, mPosY + 17.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_PEA_SPLAT);
		break;
	case PROJECTILE_ZOMBIE_CABBAGE:
		mApp->AddTodParticle(mPosX + 20.0f, mPosY + 23.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_CABBAGE_SPLAT);
		break;
	case PROJECTILE_ZOMBIE_MELON:
		mApp->AddTodParticle(mPosX + 12.0f, mPosY + 30.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_MELONSPLASH);
		break;
	case PROJECTILE_ZOMBIE_WINTERMELON:
		mApp->AddTodParticle(mPosX + 12.0f, mPosY + 30.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_WINTERMELON);
		break;
	case ProjectileType::PROJECTILE_ZOMBIE_SNOWPEA:
		mApp->AddTodParticle(mPosX - 3.0f, mPosY + 17.0f, mRenderOrder + 1, ParticleEffect::PARTICLE_SNOWPEA_SPLAT);
		break;
	default:
		break;
	}

	if (aEffect != ParticleEffect::PARTICLE_NONE)
	{
		if (theEnemy.mZombie)
		{
			float aPosX = aSplatPosX + 52.0f - theEnemy.mZombie->mX;
			float aPosY = aSplatPosY - theEnemy.mZombie->mY;
			if (theEnemy.mZombie->mZombiePhase == ZombiePhase::PHASE_SNORKEL_WALKING_IN_POOL || theEnemy.mZombie->mZombiePhase == ZombiePhase::PHASE_DOLPHIN_WALKING_IN_POOL)
			{
				aPosY += 60.0f;
			}
			if (mMotionType == ProjectileMotion::MOTION_BACKWARDS)
			{
				aPosX -= 80.0f;
			}
			else if (mPosX > theEnemy.mZombie->mX + 40 && mMotionType != ProjectileMotion::MOTION_LOBBED)
			{
				aPosX -= 60.0f;
			}

			aPosY = ClampFloat(aPosY, 20.0f, 100.0f);
			theEnemy.mZombie->AddAttachedParticle(aPosX, aPosY, aEffect);
		}
		else
		{
			mApp->AddTodParticle(aSplatPosX, aSplatPosY, mRenderOrder + 1, aEffect);
		}
	}

	Die();
}

void Projectile::Update()
{
	mProjectileAge++;
	if (mApp->mGameScene != GameScenes::SCENE_PLAYING && !mBoard->mCutScene->ShouldRunUpsellBoard())
		return;

	int aTime = 20;
	if (mProjectileType == ProjectileType::PROJECTILE_PEA || 
		mProjectileType == ProjectileType::PROJECTILE_SNOWPEA || 
		mProjectileType == ProjectileType::PROJECTILE_CABBAGE || 
		mProjectileType == ProjectileType::PROJECTILE_MELON || 
		mProjectileType == ProjectileType::PROJECTILE_WINTERMELON || 
		mProjectileType == ProjectileType::PROJECTILE_KERNEL || 
		mProjectileType == ProjectileType::PROJECTILE_BUTTER || 
		mProjectileType == ProjectileType::PROJECTILE_COBBIG || 
		mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_PEA || 
		mProjectileType == ProjectileType::PROJECTILE_SPIKE)
	{
		aTime = 0;
	}
	if (mProjectileAge > aTime)
	{
		mRenderOrder = Board::MakeRenderOrder(RenderLayer::RENDER_LAYER_PROJECTILE, mRow, 0);
	}

	if (mClickBackoffCounter > 0)
	{
		mClickBackoffCounter--;
	}
	mRotation += mRotationSpeed;

	UpdateMotion();
	if(mVelX >= 0){
		AttachmentUpdateAndMove(mAttachmentID, mPosX, mPosY + mPosZ);
	}
	else{
		SexyTransform2D aMatrix;
		aMatrix.m00 = -1; aMatrix.m01 = 0;  aMatrix.m02 = mPosX;
		aMatrix.m10 = 0;  aMatrix.m11 = 1;  aMatrix.m12 = mPosY + mPosZ;
		aMatrix.m20 = 0;  aMatrix.m21 = 0;  aMatrix.m22 = 1;
		AttachmentUpdateAndSetMatrix(mAttachmentID, aMatrix);
	}
}

void Projectile::Draw(Graphics* g)
{
	const ProjectileDefinition& aProjectileDef = GetProjectileDef();

	Image* aImage = nullptr;
	float aScale = 1.0f;
	switch (mProjectileType)
	{
	case ProjectileType::PROJECTILE_COBBIG:
		aImage = IMAGE_REANIM_COBCANNON_COB;
		aScale = 0.9f;
		break;
	case ProjectileType::PROJECTILE_PEA:
	case ProjectileType::PROJECTILE_ZOMBIE_PEA:
		aImage = IMAGE_PROJECTILEPEA;
		break;
	case ProjectileType::PROJECTILE_SNOWPEA:
	case ProjectileType::PROJECTILE_ZOMBIE_SNOWPEA:
		aImage = IMAGE_PROJECTILESNOWPEA;
		break;
	case ProjectileType::PROJECTILE_FIREBALL:
		aImage = nullptr;
		break;
	case ProjectileType::PROJECTILE_SPIKE:
		aImage = IMAGE_PROJECTILECACTUS;
		break;
	case ProjectileType::PROJECTILE_STAR:
		aImage = IMAGE_PROJECTILE_STAR;
		break;
	case ProjectileType::PROJECTILE_PUFF:
		aImage = IMAGE_PUFFSHROOM_PUFF1;
		aScale = TodAnimateCurveFloat(0, 30, mProjectileAge, 0.3f, 1.0f, TodCurves::CURVE_LINEAR);
		break;
	case ProjectileType::PROJECTILE_BASKETBALL:
		aImage = IMAGE_REANIM_ZOMBIE_CATAPULT_BASKETBALL;
		aScale = 1.1f;
		break;
	case ProjectileType::PROJECTILE_CABBAGE:
	case ProjectileType::PROJECTILE_ZOMBIE_CABBAGE:
		aImage = IMAGE_REANIM_CABBAGEPULT_CABBAGE;
		aScale = 1.0f;
		break;
	case ProjectileType::PROJECTILE_KERNEL:
		aImage = IMAGE_REANIM_CORNPULT_KERNAL;
		aScale = 0.95f;
		break;
	case ProjectileType::PROJECTILE_BUTTER:
		aImage = IMAGE_REANIM_CORNPULT_BUTTER;
		aScale = 0.8f;
		break;
	case ProjectileType::PROJECTILE_MELON:
	case ProjectileType::PROJECTILE_ZOMBIE_MELON:
		aImage = IMAGE_REANIM_MELONPULT_MELON;
		aScale = 1.0f;
		break;
	case ProjectileType::PROJECTILE_WINTERMELON:
	case ProjectileType::PROJECTILE_ZOMBIE_WINTERMELON:
		aImage = IMAGE_REANIM_WINTERMELON_PROJECTILE;
		aScale = 1.0f;
		break;
	default:
		TOD_ASSERT(false);
		break;
	}
	float aSX = aScale;

	bool aMirror = false;
	if (mMotionType == ProjectileMotion::MOTION_BEE_BACKWARDS)
	{
		aMirror = true;
	}
	if(mProjectileType != PROJECTILE_PEA && mProjectileType != PROJECTILE_ZOMBIE_PEA &&
		mProjectileType != PROJECTILE_SPIKE && mProjectileType != PROJECTILE_STAR &&
		mProjectileType != PROJECTILE_BASKETBALL && mProjectileType != PROJECTILE_COBBIG &&
		mVelX < 0
	){
		aSX *= -1;
		aMirror = true;
	}

	if (aImage)
	{
		TOD_ASSERT(aProjectileDef.mImageRow < aImage->mNumRows);
		TOD_ASSERT(mFrame < aImage->mNumCols);

		int aCelWidth = aImage->GetCelWidth();
		int aCelHeight = aImage->GetCelHeight();
		Rect aSrcRect(aCelWidth * mFrame, aCelHeight * aProjectileDef.mImageRow, aCelWidth, aCelHeight);
		if (FloatApproxEqual(mRotation, 0.0f) && FloatApproxEqual(aScale, 1.0f))
		{
			Rect aDestRect(0, 0, aCelWidth, aCelHeight);
			g->DrawImageMirror(aImage, aDestRect, aSrcRect, aMirror);
		}
		else
		{
			float aOffsetX = mPosX + aCelWidth * 0.5f;
			float aOffsetY = mPosZ + mPosY + aCelHeight * 0.5f;
			SexyTransform2D aTransform;
			TodScaleRotateTransformMatrix(aTransform, aOffsetX + mBoard->mX, aOffsetY + mBoard->mY, mRotation, aSX, aScale);
			TodBltMatrix(g, aImage, aTransform, g->mClipRect, Color::White, g->mDrawMode, aSrcRect);
		}
	}

	if (mAttachmentID != AttachmentID::ATTACHMENTID_NULL)
	{
		Graphics theParticleGraphics(*g);
		MakeParentGraphicsFrame(&theParticleGraphics);
		AttachmentDraw(mAttachmentID, &theParticleGraphics, false);
	}
}

void Projectile::DrawShadow(Graphics* g)
{
	int aCelCol = 0;
	float aScale = 1.0f;
	float aStretch = 1.0f;
	float aOffsetX = mPosX - mX;
	float aOffsetY = mPosY - mY;

	int aGridX = mBoard->PixelToGridXKeepOnBoard(mX, mY);
	bool isHighGround = false;
	if (mBoard->mGridSquareType[aGridX][mRow] == GridSquareType::GRIDSQUARE_HIGH_GROUND)
	{
		isHighGround = true;
	}
	if (mOnHighGround && !isHighGround)
	{
		aOffsetY += HIGH_GROUND_HEIGHT;
	}
	else if (!mOnHighGround && isHighGround)
	{
		aOffsetY -= HIGH_GROUND_HEIGHT;
	}

	if (mBoard->StageIsNight())
	{
		aCelCol = 1;
	}

	switch (mProjectileType)
	{
	case ProjectileType::PROJECTILE_PEA:
	case ProjectileType::PROJECTILE_ZOMBIE_PEA:
		aOffsetX += 3.0f;
		break;

	case ProjectileType::PROJECTILE_SNOWPEA:
	case ProjectileType::PROJECTILE_ZOMBIE_SNOWPEA:
		aOffsetX += -1.0f;
		aScale = 1.3f;
		break;

	case ProjectileType::PROJECTILE_STAR:
		aOffsetX += 7.0f;
		break;

	case ProjectileType::PROJECTILE_CABBAGE:
	case ProjectileType::PROJECTILE_KERNEL:
	case ProjectileType::PROJECTILE_BUTTER:
	case ProjectileType::PROJECTILE_MELON:
	case ProjectileType::PROJECTILE_WINTERMELON:
	case ProjectileType::PROJECTILE_ZOMBIE_CABBAGE:
	case ProjectileType::PROJECTILE_ZOMBIE_MELON:
	case ProjectileType::PROJECTILE_ZOMBIE_WINTERMELON:
		aOffsetX += 3.0f;
		aOffsetY += 10.0f;
		aScale = 1.6f;
		break;

	case ProjectileType::PROJECTILE_PUFF:
		return;
		
	case ProjectileType::PROJECTILE_COBBIG:
		aScale = 1.0f;
		aStretch = 3.0f;
		aOffsetX += 57.0f;
		break;

	case ProjectileType::PROJECTILE_FIREBALL:
		aScale = 1.4f;
		break;
	default:
		break;
	}

	if (mMotionType == ProjectileMotion::MOTION_LOBBED)
	{
		float aHeight = ClampFloat(-mPosZ, 0.0f, 200.0f);
		aScale *= 200.0f / (aHeight + 200.0f);
	}

	TodDrawImageCelScaledF(g, IMAGE_PEA_SHADOWS, aOffsetX, (mShadowY - mPosY + aOffsetY), aCelCol, 0, aScale * aStretch, aScale);
}

void Projectile::Die()
{
	mDead = true;

	if (mProjectileType == ProjectileType::PROJECTILE_PUFF || mProjectileType == ProjectileType::PROJECTILE_SNOWPEA ||
		mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_SNOWPEA)
	{
		AttachmentCrossFade(mAttachmentID, "FadeOut");
		AttachmentDetach(mAttachmentID);
	}
	else
	{
		AttachmentDie(mAttachmentID);
	}
}

Rect Projectile::GetProjectileRect()
{
	if (mProjectileType == ProjectileType::PROJECTILE_PEA || 
		mProjectileType == ProjectileType::PROJECTILE_SNOWPEA ||
		mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_PEA ||
		mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_SNOWPEA)
	{
		return Rect(mX - 15, mY, mWidth + 15, mHeight);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_COBBIG)
	{
		return Rect(mX + mWidth / 2 - 115, mY + mHeight / 2 - 115, 230, 230);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_MELON || mProjectileType == ProjectileType::PROJECTILE_WINTERMELON
	)
	{
		return Rect(mX + 20, mY, 60, mHeight);
	}
	else if(mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_MELON || mProjectileType == ProjectileType::PROJECTILE_ZOMBIE_WINTERMELON){
		return Rect(mX - 40, mY, 60, mHeight);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_FIREBALL)
	{
		return Rect(mX, mY, mWidth - 10, mHeight);
	}
	else if (mProjectileType == ProjectileType::PROJECTILE_SPIKE)
	{
		return Rect(mX - 25, mY, mWidth + 25, mHeight);
	}
	else
	{
		return Rect(mX, mY, mWidth, mHeight);
	}
}

void Projectile::ConvertToFireball(int theGridX)
{
	if (mHitTorchwoodGridX == theGridX)
		return;

	mProjectileType = ProjectileType::PROJECTILE_FIREBALL;
	mHitTorchwoodGridX = theGridX;
	mApp->PlayFoley(FoleyType::FOLEY_FIREPEA);

	float aOffsetX = -25.0f;
	float aOffsetY = -25.0f;
	Reanimation* aFirePeaReanim = mApp->AddReanimation(0.0f, 0.0f, 0, ReanimationType::REANIM_FIRE_PEA);
	if (mMotionType == ProjectileMotion::MOTION_BACKWARDS)
	{
		aFirePeaReanim->OverrideScale(-1.0f, 1.0f);
		aOffsetX += 80.0f;
	}

	aFirePeaReanim->SetPosition(mPosX + aOffsetX, mPosY + aOffsetY);
	aFirePeaReanim->mLoopType = ReanimLoopType::REANIM_LOOP;
	aFirePeaReanim->mAnimRate = RandRangeFloat(50.0f, 80.0f);
	AttachReanim(mAttachmentID, aFirePeaReanim, aOffsetX, aOffsetY);
}

void Projectile::ConvertToPea(int theGridX)
{
	if (mHitTorchwoodGridX == theGridX)
		return;

	AttachmentDie(mAttachmentID);
	mProjectileType = ProjectileType::PROJECTILE_PEA;
	mHitTorchwoodGridX = theGridX;
	mApp->PlayFoley(FoleyType::FOLEY_THROW);
}

const ProjectileDefinition& Projectile::GetProjectileDef()
{
	const ProjectileDefinition& aProjectileDef = gProjectileDefinition[mProjectileType];
	TOD_ASSERT(aProjectileDef.mProjectileType == mProjectileType);

	return aProjectileDef;
}

bool Projectile::IsZombieProjectile(){
	return mProjectileType == PROJECTILE_BASKETBALL || mProjectileType == PROJECTILE_ZOMBIE_CABBAGE || mProjectileType == PROJECTILE_ZOMBIE_PEA || 
	mProjectileType == PROJECTILE_ZOMBIE_MELON || mProjectileType == PROJECTILE_ZOMBIE_SNOWPEA || mProjectileType == PROJECTILE_ZOMBIE_WINTERMELON;
}