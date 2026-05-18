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

#include "../Board.h"
#include "../Plant.h"
#include "../Zombie.h"
#include "GameButton.h"
#include "../SeedPacket.h"
#include "../../LawnApp.h"
#include "AllowedZombieDialog.h"
#include "../../Resources.h"
#include "../System/Music.h"
#include "../../GameConstants.h"
#include "../System/PlayerInfo.h"
#include "../System/PoolEffect.h"
#include "../System/ReanimationLawn.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "widget/WidgetManager.h"
#include "graphics/ImageFont.h"
#include "LawnDialog.h"

ZombieType gChoosableZombieTypes[NUM_CHOOSABLE_ZOMBIES] = {
    ZOMBIE_POLEVAULTER,
    ZOMBIE_NEWSPAPER,
    ZOMBIE_DOOR,
    ZOMBIE_FOOTBALL,
    ZOMBIE_DANCER,
    ZOMBIE_SNORKEL,
    ZOMBIE_ZAMBONI,
    ZOMBIE_BOBSLED,
    ZOMBIE_DOLPHIN_RIDER,
    ZOMBIE_JACK_IN_THE_BOX,
    ZOMBIE_BALLOON,
    ZOMBIE_DIGGER,
    ZOMBIE_POGO,
    ZOMBIE_BUNGEE,
    ZOMBIE_LADDER,
    ZOMBIE_CATAPULT,
    ZOMBIE_GARGANTUAR,
    ZOMBIE_REDEYE_GARGANTUAR,
    ZOMBIE_GIGA_FOOTBALL,
    ZOMBIE_DOOR_PAIL,
    ZOMBIE_JACKSON,
    ZOMBIE_BLUEOON,
    ZOMBIE_BOSS,
    ZOMBIE_PEA_HEAD,
    ZOMBIE_WALLNUT_HEAD,
    ZOMBIE_JALAPENO_HEAD,
    ZOMBIE_GATLING_HEAD,
    ZOMBIE_SQUASH_HEAD,
    ZOMBIE_TALLNUT_HEAD,
};

Rect ContentRect(50, 114, 680, 356);

AllowedZombieDialog::AllowedZombieDialog(LawnApp* theApp, bool* allowedZombies) : Dialog(nullptr, nullptr, Dialogs::DIALOG_AllowedZombie, true, "Allowed Zombies", "", "", Dialog::BUTTONS_NONE)
{
	mApp = theApp;
	mZombieAllowed = allowedZombies;
	for (size_t i = 0; i < LENGTH(mZombiePerfTest); i++) mZombiePerfTest[i] = nullptr;

	SetColor(Dialog::COLOR_BUTTON_TEXT, Color(255, 255, 100));

    SetColor(0, { 0xE0,0xBB,0x62 });
    SetColor(1, { 0xE0,0xBB,0x62 });
    SetHeaderFont(Sexy::FONT_DWARVENTODCRAFT24);
	
    mWidth = 693;
    mHeight = 584;
	mScrollY = 0;
    mCloseButton = MakeButton(AllowedZombieDialog::ALLOWEDZOMBIE_BUTTON_CLOSE, this, "[CLOSE_BUTTON]");
}

AllowedZombieDialog::~AllowedZombieDialog()
{
	delete mCloseButton;
	ClearPlantsAndZombies();
}

void AllowedZombieDialog::AddedToManager(Sexy::WidgetManager* theWidgetManager)
{
    Dialog::AddedToManager(theWidgetManager);
    AddWidget(mCloseButton);
}

void AllowedZombieDialog::ClearPlantsAndZombies()
{
	for (Zombie* &aZombie : mZombiePerfTest)
	{
		if (aZombie)
		{
			aZombie->DieNoLoot();
			delete aZombie;
		}
		aZombie = nullptr;
	}
}

void AllowedZombieDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Dialog::RemovedFromManager(theWidgetManager);
	ClearPlantsAndZombies();
    RemoveWidget(mCloseButton);
}

void AllowedZombieDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
    Dialog::Resize(theX, theY, theWidth, theHeight);

    mContentInsets = Insets(36, 35, 46, 36);
    int aButtonAreaY = mHeight - mContentInsets.mBottom - mBackgroundInsets.mBottom - IMAGE_BUTTON_LEFT->mHeight + 2;
    aButtonAreaY -= 10;

    mCloseButton->Resize(242, aButtonAreaY, 209, 46);
}

void AllowedZombieDialog::Update()
{
	for (Zombie* aZombie : mZombiePerfTest)
	{
		if (aZombie)
		{
			aZombie->Update();
		}
	}

	int aMouseX = mApp->mWidgetManager->mLastMouseX - mX;
	int aMouseY = mApp->mWidgetManager->mLastMouseY - mY;
	if (ZombieHitTest(aMouseX, aMouseY) != -1)
	{
		mApp->SetCursor(CURSOR_HAND);
	}
	else
	{
		mApp->SetCursor(CURSOR_POINTER);
	}
	MarkDirty();
}

// GOTY @Patoke: 0x403DE0
void AllowedZombieDialog::DrawZombies(Graphics* g)
{
	int aZombieMouseOn = ZombieHitTest(mApp->mWidgetManager->mLastMouseX - mX, mApp->mWidgetManager->mLastMouseY - mY);
	g->SetClipRect(ContentRect);
	for (int i = 0; i < NUM_CHOOSABLE_ZOMBIES; i++)
	{
		ZombieType aZombieType = gChoosableZombieTypes[i];
		bool chosen = mZombieAllowed[aZombieType];
		int aPosX, aPosY;
		GetZombiePosition(i, aPosX, aPosY);
		if (aZombieType != ZombieType::ZOMBIE_INVALID)
		{
			if (!ZombieIsShown(aZombieType))
				g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEBLANK, aPosX, aPosY);
			else
			{
				if(!chosen){
					g->SetColor(Color(128, 128, 128));
					g->SetColorizeImages(true);
				}
				g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW, aPosX, aPosY);
				g->SetColorizeImages(false);
				if (i == aZombieMouseOn)
				{
					g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
					g->SetColor(Color(255, 255, 255, 48));
					g->SetColorizeImages(true);
					g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW, aPosX, aPosY);
					g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
					g->SetColorizeImages(false);
				}

				ZombieType aZombieTypeToDraw = aZombieType;
				Graphics aZombieGraphics = Graphics(*g);
				aZombieGraphics.SetClipRect(ContentRect.Intersection(Rect(aPosX + 2, aPosY + 2, 72, 72)));
				aZombieGraphics.Translate(aPosX + 1, aPosY - 6);
				aZombieGraphics.mScaleX = 0.5f;
				aZombieGraphics.mScaleY = 0.5f;
				switch (aZombieType)
				{
				case ZombieType::ZOMBIE_POLEVAULTER:
					aZombieGraphics.TranslateF(2, -3);
					aZombieTypeToDraw = ZombieType::ZOMBIE_CACHED_POLEVAULTER_WITH_POLE;		break;
				case ZombieType::ZOMBIE_FLAG:			aZombieGraphics.TranslateF(2, 10);	break;
				case ZombieType::ZOMBIE_TRAFFIC_CONE:	aZombieGraphics.TranslateF(0, 12);	break;
				case ZombieType::ZOMBIE_PAIL:			aZombieGraphics.TranslateF(0, 9);		break;
				case ZombieType::ZOMBIE_DOOR_PAIL:			aZombieGraphics.TranslateF(0, 9);		break;
				case ZombieType::ZOMBIE_FOOTBALL:		aZombieGraphics.TranslateF(-15, -1);	break;
				case ZombieType::ZOMBIE_GIGA_FOOTBALL:		aZombieGraphics.TranslateF(-15, -1);	break;
				case ZombieType::ZOMBIE_ZAMBONI:		aZombieGraphics.TranslateF(0, 3);		break;
				case ZombieType::ZOMBIE_DOLPHIN_RIDER:	aZombieGraphics.TranslateF(-2, -10);	break;
				case ZombieType::ZOMBIE_POGO:			aZombieGraphics.TranslateF(0, -3);	break;
				case ZombieType::ZOMBIE_GARGANTUAR:		aZombieGraphics.TranslateF(15, 17);	break;
				case ZombieType::ZOMBIE_REDEYE_GARGANTUAR:		aZombieGraphics.TranslateF(15, 17);	break;
				case ZombieType::ZOMBIE_BUNGEE:			aZombieGraphics.TranslateF(-4, 3);	break;
				case ZombieType::ZOMBIE_DANCER:			aZombieGraphics.TranslateF(0, 15);	break;
				case ZombieType::ZOMBIE_SNORKEL:		aZombieGraphics.TranslateF(-10, 0);	break;
				case ZombieType::ZOMBIE_YETI:			aZombieGraphics.TranslateF(0, 4);		break;
				case ZombieType::ZOMBIE_CATAPULT:		aZombieGraphics.TranslateF(-24, -1);	break;
				case ZombieType::ZOMBIE_BOBSLED:		aZombieGraphics.TranslateF(0, -8);	break;
				case ZombieType::ZOMBIE_LADDER:			aZombieGraphics.TranslateF(0, -3);	break;
				default: break;
				}
				
				if(!chosen){
					aZombieGraphics.SetColor(Color(128, 128, 128));
					aZombieGraphics.SetColorizeImages(true);
				}
				if (ZombieHasSilhouette(aZombieType))
				{
					aZombieGraphics.SetColor(Color(0, 0, 0, 40));
					aZombieGraphics.SetColorizeImages(true);
				}
				mApp->mReanimatorCache->DrawCachedZombie(&aZombieGraphics, 0, 0, aZombieTypeToDraw);
				aZombieGraphics.SetColorizeImages(false);

				
				if(!chosen){
					g->SetColor(Color(128, 128, 128));
					g->SetColorizeImages(true);
				}
				g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW2, aPosX, aPosY);
				g->SetColorizeImages(false);
				if (i == aZombieMouseOn)
				{
					g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
					g->SetColor(Color(255, 255, 255, 48));
					g->SetColorizeImages(true);
					g->DrawImage(Sexy::IMAGE_ALMANAC_ZOMBIEWINDOW2, aPosX, aPosY);
					g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
					g->SetColorizeImages(false);
				}
			}
		}
	}
}

void AllowedZombieDialog::Draw(Graphics* g)
{
	Image* aBottomLeftImage = IMAGE_DIALOG_BOTTOMLEFT;
    Image* aBottomMiddleImage = IMAGE_DIALOG_BOTTOMMIDDLE;
    Image* aBottomRightImage = IMAGE_DIALOG_BOTTOMRIGHT;



    int aRepeatX = (mWidth - IMAGE_DIALOG_TOPRIGHT->mWidth - IMAGE_DIALOG_TOPLEFT->mWidth) / IMAGE_DIALOG_TOPMIDDLE->mWidth;
    int aRepeatY = (mHeight - IMAGE_DIALOG_TOPLEFT->mHeight - aBottomLeftImage->mHeight - DIALOG_HEADER_OFFSET) / IMAGE_DIALOG_CENTERLEFT->mHeight;

    int aPosX = 0;
    int aPosY = DIALOG_HEADER_OFFSET;
    g->DrawImage(IMAGE_DIALOG_TOPLEFT, aPosX, aPosY);
    aPosX += IMAGE_DIALOG_TOPLEFT->mWidth;
    for (int i = 0; i < aRepeatX; i++)
    {
        g->DrawImage(IMAGE_DIALOG_TOPMIDDLE, aPosX, aPosY);
        aPosX += IMAGE_DIALOG_TOPMIDDLE->mWidth;
    }
    g->DrawImage(IMAGE_DIALOG_TOPRIGHT, aPosX, aPosY);

    aPosY += IMAGE_DIALOG_TOPRIGHT->mHeight;
    for (int y = 0; y < aRepeatY; y++)
    {
        aPosX = 0;

        g->DrawImage(IMAGE_DIALOG_CENTERLEFT, aPosX, aPosY);
        aPosX += IMAGE_DIALOG_CENTERLEFT->mWidth;

        for (int x = 0; x < aRepeatX; x++)
        {
            g->DrawImage(IMAGE_DIALOG_CENTERMIDDLE, aPosX, aPosY);
            aPosX += IMAGE_DIALOG_CENTERMIDDLE->mWidth;
        }

        g->DrawImage(IMAGE_DIALOG_CENTERRIGHT, aPosX, aPosY);
        aPosY += IMAGE_DIALOG_CENTERLEFT->mHeight;
    }

	aPosX = 0;
    g->DrawImage(aBottomLeftImage, aPosX, aPosY);
    aPosX += aBottomLeftImage->mWidth;
    for (int i = 0; i < aRepeatX; i++)
    {
        g->DrawImage(aBottomMiddleImage, aPosX, aPosY);
        aPosX += aBottomMiddleImage->mWidth;
    }
    g->DrawImage(aBottomRightImage, aPosX, aPosY);
    g->DrawImage(IMAGE_DIALOG_HEADER, (mWidth - IMAGE_DIALOG_HEADER->mWidth) / 2 - 5, 0);

    int aFontY = mContentInsets.mTop + mBackgroundInsets.mTop + DIALOG_HEADER_OFFSET;
    if (mDialogHeader.size() > 0)
    {
        int aOffsetY = aFontY - mHeaderFont->GetAscentPadding() + mHeaderFont->GetAscent();
        g->SetFont(mHeaderFont);
        g->SetColor(mColors[Dialog::COLOR_HEADER]);
        WriteCenteredLine(g, aOffsetY, mDialogHeader);
        aFontY = aOffsetY - mHeaderFont->GetAscent() + mHeaderFont->GetHeight() + mSpaceAfterHeader;
    }

	for (Zombie* aZombie : mZombiePerfTest)
	{
		if (aZombie)
		{
			Graphics aTestGraphics = Graphics(*g);
			aZombie->Draw(&aTestGraphics);
		}
	}

	DrawZombies(g);
}

// GOTY @Patoke: 0x404C50
bool AllowedZombieDialog::ZombieIsShown(ZombieType theZombieType)
{
	// 试玩模式下，仅展示潜水僵尸及其之前出现的僵尸
	if (mApp->IsTrialStageLocked() && theZombieType > ZombieType::ZOMBIE_SNORKEL)
		return false;

	// 对于雪人僵尸，要求其可以在刷怪中出现（已经到达或完成冒险模式二周目 4-10 关卡），
	// 或已得知其存在但未解锁其形象（已经完成冒险模式一周目 4-10 关卡，但未到达二周目 4-10 关卡）
	if (theZombieType == ZombieType::ZOMBIE_YETI)
		return mApp->CanSpawnYetis() || ZombieHasSilhouette(ZombieType::ZOMBIE_YETI);

	// 对于冒险模式中出现的僵尸
	if (theZombieType <= ZombieType::ZOMBIE_BOSS)
	{
		// 冒险模式一周目完成后，图鉴展示所有僵尸
		if (mApp->HasFinishedAdventure())
			return true;

		int aLevel = mApp->mPlayerInfo->GetLevel();
		int aStart = GetZombieDefinition(theZombieType).mStartingLevel;
		// 要求已经达到僵尸首次出现的关卡
		// 对于不能通过自然刷怪出现的僵尸（小鬼僵尸、雪橇僵尸小队、伴舞僵尸），额外要求已通过其首次出现的关卡或已击败过该僵尸
		return aStart <= aLevel && (aStart != aLevel || !Board::IsZombieTypeSpawnedOnly(theZombieType));
	}

	return true;
}

bool AllowedZombieDialog::ZombieHasSilhouette(ZombieType theZombieType)
{
	// 除雪人僵尸以外的其他僵尸，或者雪人僵尸已经可以刷出（已经到达或完成冒险模式二周目 4-10 关卡），则不会显示为剪影
	if (theZombieType != ZombieType::ZOMBIE_YETI || mApp->CanSpawnYetis())
		return false;

	// 排除上述情况后，若已完成雪人僵尸出现的关卡（冒险模式一周目 4-10 关卡），则雪人僵尸显示为剪影
	return mApp->HasFinishedAdventure() || mApp->mPlayerInfo->GetLevel() > GetZombieDefinition(ZombieType::ZOMBIE_YETI).mStartingLevel;
}

// GOTY @Patoke: 0x404D50
bool AllowedZombieDialog::ZombieHasDescription(ZombieType theZombieType)
{
	int aLevel = mApp->mPlayerInfo->GetLevel();
	int aStart = GetZombieDefinition(theZombieType).mStartingLevel;

	// 对于雪人僵尸
	if (theZombieType == ZombieType::ZOMBIE_YETI)
	{
		// 当雪人僵尸不可在刷怪中出现时（冒险模式二周目 4-10 关卡之前），不显示僵尸描述
		if (!mApp->CanSpawnYetis())
			return false;
		// 从第三周目开始，总是显示雪人僵尸的描述
		if (mApp->mPlayerInfo->mFinishedAdventure >= 2)
			return true;
	}
	// 对于雪人僵尸外的其他僵尸，当冒险模式已完成时，总是显示僵尸的描述
	else if (mApp->HasFinishedAdventure())
		return true;

	// 雪人僵尸在二周目 4-10 关卡至三周目之间，或其他僵尸在冒险模式一周目中的情况，
	// 要求已经达到僵尸首次出现的关卡，且已通过其首次出现的关卡或已击败过该僵尸
	return aStart <= aLevel && (aStart != aLevel);
}

void AllowedZombieDialog::GetZombiePosition(int num, int& x, int& y)
{
	x = num % 7 * 85 + 52;
	y = num / 7 * 80 + 116 + mScrollY;
}

// GOTY @Patoke: 0x404DD0
int AllowedZombieDialog::ZombieHitTest(int x, int y)
{
	if(!ContentRect.Contains(x, y)) return -1;
	if (mMouseVisible)
	{
		for (int i = 0; i < NUM_CHOOSABLE_ZOMBIES; i++)
		{
			ZombieType aZombieType = gChoosableZombieTypes[i];
			// @Patoke: added IsShown check
			if (ZombieIsShown(aZombieType))
			{
				int aZombieX, aZombieY;
				GetZombiePosition(i, aZombieX, aZombieY);
				if (Rect(aZombieX, aZombieY, 76, 76).Contains(x, y))
					return i;
			}
		}
	}
	return -1;
}

void AllowedZombieDialog::MouseUp(int x, int y, int theClickCount)
{
	int pik = ZombieHitTest(x, y);
	if(pik != -1){
		ZombieType aZombieType = gChoosableZombieTypes[pik];
		if (aZombieType != ZombieType::ZOMBIE_INVALID)
		{
			mZombieAllowed[aZombieType] = !mZombieAllowed[aZombieType];
			if(aZombieType == ZOMBIE_BOBSLED && mZombieAllowed[ZOMBIE_BOBSLED]) mZombieAllowed[ZOMBIE_ZAMBONI] = true;
			if(aZombieType == ZOMBIE_ZAMBONI && !mZombieAllowed[ZOMBIE_ZAMBONI]) mZombieAllowed[ZOMBIE_BOBSLED] = false;
			mApp->PlaySample(Sexy::SOUND_TAP);
		}
	}
}

void AllowedZombieDialog::MouseWheel(int theDelta){
	Widget::MouseWheel(theDelta);
	if(ContentRect.Contains(mApp->mWidgetManager->mLastMouseX - mX, mApp->mWidgetManager->mLastMouseY - mY)){
		if (theDelta > 0){
			mScrollY += 10;
			if(mScrollY >= 0) mScrollY = 0;
		}
		else if(theDelta < 0){
			mScrollY -= 10;
			int Ymin = (ContentRect.mY + ContentRect.mHeight) - (((NUM_CHOOSABLE_ZOMBIES - 1) / 7) * 80 + 192);
			if(mScrollY <= Ymin) mScrollY = Ymin;
		}
	}
}

// GOTY @Patoke: 0x404F10
void AllowedZombieDialog::MouseDown(int x, int y, int theClickCount)
{
	
}

void AllowedZombieDialog::ButtonPress(int theId)
{
    (void)theId;
    mApp->PlaySample(SOUND_GRAVEBUTTON);
}

void AllowedZombieDialog::ButtonDepress(int theId)
{
    Dialog::ButtonDepress(theId);

    switch (theId)
    {
		case AllowedZombieDialog::ALLOWEDZOMBIE_BUTTON_CLOSE:
		{
			mApp->KillDialog(Dialogs::DIALOG_AllowedZombie);
			break;
		}
	}
}
