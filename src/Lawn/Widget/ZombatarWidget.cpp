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

#include "ZombatarWidget.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "GameButton.h"
#include "GameSelector.h"
#include "../../Sexy.TodLib/TodDebug.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../SexyAppFramework/graphics/Graphics.h"
#include "../../SexyAppFramework/graphics/Image.h"
#include "../../SexyAppFramework/graphics/MemoryImage.h"
#include "../../SexyAppFramework/widget/WidgetManager.h"
#include "../../SexyAppFramework/widget/DialogButton.h"
#include "../../SexyAppFramework/widget/Dialog.h"
#include "SexyAppFramework/graphics/Color.h"
#include "../System/PlayerInfo.h"
#include "../System/ZombatarComposer.h"
#include <algorithm>

// Category resource tables — use function-local statics so they initialise
// after resources have been loaded (lazy initialisation).

static Image** GetHairImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_HAIR_1, IMAGE_ZOMBATAR_HAIR_2, IMAGE_ZOMBATAR_HAIR_3,
		IMAGE_ZOMBATAR_HAIR_4, IMAGE_ZOMBATAR_HAIR_5, IMAGE_ZOMBATAR_HAIR_6,
		IMAGE_ZOMBATAR_HAIR_7, IMAGE_ZOMBATAR_HAIR_8, IMAGE_ZOMBATAR_HAIR_9,
		IMAGE_ZOMBATAR_HAIR_10, IMAGE_ZOMBATAR_HAIR_11, IMAGE_ZOMBATAR_HAIR_12,
		IMAGE_ZOMBATAR_HAIR_13, IMAGE_ZOMBATAR_HAIR_14, IMAGE_ZOMBATAR_HAIR_15,
		IMAGE_ZOMBATAR_HAIR_16
	};
	return sImages;
}

static Image** GetHairMasks()
{
	static Image* sMasks[] = {
		IMAGE_ZOMBATAR_HAIR_1_MASK, IMAGE_ZOMBATAR_HAIR_2_MASK, nullptr,
		nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr,
		nullptr, IMAGE_ZOMBATAR_HAIR_11_MASK, IMAGE_ZOMBATAR_HAIR_12_MASK,
		IMAGE_ZOMBATAR_HAIR_13_MASK, IMAGE_ZOMBATAR_HAIR_14_MASK, IMAGE_ZOMBATAR_HAIR_15_MASK,
		nullptr
	};
	return sMasks;
}

static Image** GetFacialHairImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_FACIALHAIR_1, IMAGE_ZOMBATAR_FACIALHAIR_2, IMAGE_ZOMBATAR_FACIALHAIR_3,
		IMAGE_ZOMBATAR_FACIALHAIR_4, IMAGE_ZOMBATAR_FACIALHAIR_5, IMAGE_ZOMBATAR_FACIALHAIR_6,
		IMAGE_ZOMBATAR_FACIALHAIR_7, IMAGE_ZOMBATAR_FACIALHAIR_8, IMAGE_ZOMBATAR_FACIALHAIR_9,
		IMAGE_ZOMBATAR_FACIALHAIR_10, IMAGE_ZOMBATAR_FACIALHAIR_11, IMAGE_ZOMBATAR_FACIALHAIR_12,
		IMAGE_ZOMBATAR_FACIALHAIR_13, IMAGE_ZOMBATAR_FACIALHAIR_14, IMAGE_ZOMBATAR_FACIALHAIR_15,
		IMAGE_ZOMBATAR_FACIALHAIR_16, IMAGE_ZOMBATAR_FACIALHAIR_17, IMAGE_ZOMBATAR_FACIALHAIR_18,
		IMAGE_ZOMBATAR_FACIALHAIR_19, IMAGE_ZOMBATAR_FACIALHAIR_20, IMAGE_ZOMBATAR_FACIALHAIR_21,
		IMAGE_ZOMBATAR_FACIALHAIR_22, IMAGE_ZOMBATAR_FACIALHAIR_23, IMAGE_ZOMBATAR_FACIALHAIR_24
	};
	return sImages;
}

static Image** GetFacialHairMasks()
{
	static Image* sMasks[] = {
		IMAGE_ZOMBATAR_FACIALHAIR_1_MASK, nullptr, nullptr,
		IMAGE_ZOMBATAR_FACIALHAIR_4_MASK, nullptr, nullptr,
		nullptr, IMAGE_ZOMBATAR_FACIALHAIR_8_MASK, IMAGE_ZOMBATAR_FACIALHAIR_9_MASK,
		IMAGE_ZOMBATAR_FACIALHAIR_10_MASK, IMAGE_ZOMBATAR_FACIALHAIR_11_MASK, IMAGE_ZOMBATAR_FACIALHAIR_12_MASK,
		nullptr, IMAGE_ZOMBATAR_FACIALHAIR_14_MASK, IMAGE_ZOMBATAR_FACIALHAIR_15_MASK,
		IMAGE_ZOMBATAR_FACIALHAIR_16_MASK, nullptr, IMAGE_ZOMBATAR_FACIALHAIR_18_MASK,
		nullptr, nullptr, IMAGE_ZOMBATAR_FACIALHAIR_21_MASK,
		IMAGE_ZOMBATAR_FACIALHAIR_22_MASK, IMAGE_ZOMBATAR_FACIALHAIR_23_MASK, IMAGE_ZOMBATAR_FACIALHAIR_24_MASK
	};
	return sMasks;
}

static Image** GetHatsImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_HATS_1, IMAGE_ZOMBATAR_HATS_2, IMAGE_ZOMBATAR_HATS_3,
		IMAGE_ZOMBATAR_HATS_4, IMAGE_ZOMBATAR_HATS_5, IMAGE_ZOMBATAR_HATS_6,
		IMAGE_ZOMBATAR_HATS_7, IMAGE_ZOMBATAR_HATS_8, IMAGE_ZOMBATAR_HATS_9,
		IMAGE_ZOMBATAR_HATS_10, IMAGE_ZOMBATAR_HATS_11, IMAGE_ZOMBATAR_HATS_12,
		IMAGE_ZOMBATAR_HATS_13, IMAGE_ZOMBATAR_HATS_14
	};
	return sImages;
}

static Image** GetHatsMasks()
{
	static Image* sMasks[] = {
		IMAGE_ZOMBATAR_HATS_1_MASK, nullptr, IMAGE_ZOMBATAR_HATS_3_MASK,
		nullptr, nullptr, IMAGE_ZOMBATAR_HATS_6_MASK,
		IMAGE_ZOMBATAR_HATS_7_MASK, IMAGE_ZOMBATAR_HATS_8_MASK, IMAGE_ZOMBATAR_HATS_9_MASK,
		nullptr, IMAGE_ZOMBATAR_HATS_11_MASK, nullptr,
		nullptr, nullptr
	};
	return sMasks;
}

static Image** GetEyewearImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_EYEWEAR_1, IMAGE_ZOMBATAR_EYEWEAR_2, IMAGE_ZOMBATAR_EYEWEAR_3,
		IMAGE_ZOMBATAR_EYEWEAR_4, IMAGE_ZOMBATAR_EYEWEAR_5, IMAGE_ZOMBATAR_EYEWEAR_6,
		IMAGE_ZOMBATAR_EYEWEAR_7, IMAGE_ZOMBATAR_EYEWEAR_8, IMAGE_ZOMBATAR_EYEWEAR_9,
		IMAGE_ZOMBATAR_EYEWEAR_10, IMAGE_ZOMBATAR_EYEWEAR_11, IMAGE_ZOMBATAR_EYEWEAR_12,
		IMAGE_ZOMBATAR_EYEWEAR_13, IMAGE_ZOMBATAR_EYEWEAR_14, IMAGE_ZOMBATAR_EYEWEAR_15,
		IMAGE_ZOMBATAR_EYEWEAR_16
	};
	return sImages;
}

static Image** GetEyewearMasks()
{
	static Image* sMasks[] = {
		IMAGE_ZOMBATAR_EYEWEAR_1_MASK, IMAGE_ZOMBATAR_EYEWEAR_2_MASK, IMAGE_ZOMBATAR_EYEWEAR_3_MASK,
		IMAGE_ZOMBATAR_EYEWEAR_4_MASK, IMAGE_ZOMBATAR_EYEWEAR_5_MASK, IMAGE_ZOMBATAR_EYEWEAR_6_MASK,
		IMAGE_ZOMBATAR_EYEWEAR_7_MASK, IMAGE_ZOMBATAR_EYEWEAR_8_MASK, IMAGE_ZOMBATAR_EYEWEAR_9_MASK,
		IMAGE_ZOMBATAR_EYEWEAR_10_MASK, IMAGE_ZOMBATAR_EYEWEAR_11_MASK, IMAGE_ZOMBATAR_EYEWEAR_12_MASK,
		nullptr, nullptr, nullptr,
		nullptr
	};
	return sMasks;
}

static Image** GetClothesImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_CLOTHES_1, IMAGE_ZOMBATAR_CLOTHES_2, IMAGE_ZOMBATAR_CLOTHES_3,
		IMAGE_ZOMBATAR_CLOTHES_4, IMAGE_ZOMBATAR_CLOTHES_5, IMAGE_ZOMBATAR_CLOTHES_6,
		IMAGE_ZOMBATAR_CLOTHES_7, IMAGE_ZOMBATAR_CLOTHES_8, IMAGE_ZOMBATAR_CLOTHES_9,
		IMAGE_ZOMBATAR_CLOTHES_10, IMAGE_ZOMBATAR_CLOTHES_11, IMAGE_ZOMBATAR_CLOTHES_12
	};
	return sImages;
}

static Image** GetAccessoryImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_ACCESSORY_1, IMAGE_ZOMBATAR_ACCESSORY_2, IMAGE_ZOMBATAR_ACCESSORY_3,
		IMAGE_ZOMBATAR_ACCESSORY_4, IMAGE_ZOMBATAR_ACCESSORY_5, IMAGE_ZOMBATAR_ACCESSORY_6,
		IMAGE_ZOMBATAR_ACCESSORY_7, IMAGE_ZOMBATAR_ACCESSORY_8, IMAGE_ZOMBATAR_ACCESSORY_9,
		IMAGE_ZOMBATAR_ACCESSORY_10, IMAGE_ZOMBATAR_ACCESSORY_11, IMAGE_ZOMBATAR_ACCESSORY_12,
		IMAGE_ZOMBATAR_ACCESSORY_13, IMAGE_ZOMBATAR_ACCESSORY_14, IMAGE_ZOMBATAR_ACCESSORY_15,
		IMAGE_ZOMBATAR_ACCESSORY_16
	};
	return sImages;
}

static Image** GetTidbitsImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_TIDBITS_1, IMAGE_ZOMBATAR_TIDBITS_2, IMAGE_ZOMBATAR_TIDBITS_3,
		IMAGE_ZOMBATAR_TIDBITS_4, IMAGE_ZOMBATAR_TIDBITS_5, IMAGE_ZOMBATAR_TIDBITS_6,
		IMAGE_ZOMBATAR_TIDBITS_7, IMAGE_ZOMBATAR_TIDBITS_8, IMAGE_ZOMBATAR_TIDBITS_9,
		IMAGE_ZOMBATAR_TIDBITS_10, IMAGE_ZOMBATAR_TIDBITS_11, IMAGE_ZOMBATAR_TIDBITS_12,
		IMAGE_ZOMBATAR_TIDBITS_13, IMAGE_ZOMBATAR_TIDBITS_14
	};
	return sImages;
}

static Image** GetBackdropImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_BACKGROUND_MENU, IMAGE_ZOMBATAR_BACKGROUND_ROOF,
		IMAGE_ZOMBATAR_BACKGROUND_CRAZYDAVE, IMAGE_ZOMBATAR_BACKGROUND_MENU_DOS,
		IMAGE_ZOMBATAR_BACKGROUND_BLANK
	};
	return sImages;
}

static Image* GetCategoryButtonImage(int category)
{
	switch (category)
	{
	case ZOMBATAR_CATEGORY_SKIN:        return IMAGE_ZOMBATAR_SKIN_BUTTON;
	case ZOMBATAR_CATEGORY_HAIR:        return IMAGE_ZOMBATAR_HAIR_BUTTON;
	case ZOMBATAR_CATEGORY_FACIAL_HAIR: return IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON;
	case ZOMBATAR_CATEGORY_HATS:        return IMAGE_ZOMBATAR_HATS_BUTTON;
	case ZOMBATAR_CATEGORY_EYEWEAR:     return IMAGE_ZOMBATAR_EYEWEAR_BUTTON;
	case ZOMBATAR_CATEGORY_CLOTHES:     return IMAGE_ZOMBATAR_CLOTHES_BUTTON;
	case ZOMBATAR_CATEGORY_ACCESSORIES: return IMAGE_ZOMBATAR_ACCESSORY_BUTTON;
	case ZOMBATAR_CATEGORY_TIDBITS:     return IMAGE_ZOMBATAR_TIDBITS_BUTTON;
	case ZOMBATAR_CATEGORY_BACKDROPS:   return IMAGE_ZOMBATAR_BACKDROPS_BUTTON;
	default: return nullptr;
	}
}

static Image* GetCategoryButtonHighlight(int category)
{
	switch (category)
	{
	case ZOMBATAR_CATEGORY_SKIN:        return IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT;
	case ZOMBATAR_CATEGORY_HAIR:        return IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT;
	case ZOMBATAR_CATEGORY_FACIAL_HAIR: return IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT;
	case ZOMBATAR_CATEGORY_HATS:        return IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT;
	case ZOMBATAR_CATEGORY_EYEWEAR:     return IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT;
	case ZOMBATAR_CATEGORY_CLOTHES:     return IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT;
	case ZOMBATAR_CATEGORY_ACCESSORIES: return IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT;
	case ZOMBATAR_CATEGORY_TIDBITS:     return IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT;
	case ZOMBATAR_CATEGORY_BACKDROPS:   return IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT;
	default: return nullptr;
	}
}

static Image* GetCategoryButtonOver(int category)
{
	switch (category)
	{
	case ZOMBATAR_CATEGORY_HAIR:        return IMAGE_ZOMBATAR_HAIR_BUTTON_OVER;
	case ZOMBATAR_CATEGORY_FACIAL_HAIR: return IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_OVER;
	case ZOMBATAR_CATEGORY_HATS:        return IMAGE_ZOMBATAR_HATS_BUTTON_OVER;
	case ZOMBATAR_CATEGORY_EYEWEAR:     return IMAGE_ZOMBATAR_EYEWEAR_BUTTON_OVER;
	case ZOMBATAR_CATEGORY_CLOTHES:     return IMAGE_ZOMBATAR_CLOTHES_BUTTON_OVER;
	case ZOMBATAR_CATEGORY_ACCESSORIES: return IMAGE_ZOMBATAR_ACCESSORY_BUTTON_OVER;
	case ZOMBATAR_CATEGORY_TIDBITS:     return IMAGE_ZOMBATAR_TIDBITS_BUTTON_OVER;
	case ZOMBATAR_CATEGORY_BACKDROPS:   return IMAGE_ZOMBATAR_BACKDROPS_BUTTON_OVER;
	default: return nullptr;
	}
}

static int gCategoryItemCounts[NUM_ZOMBATAR_CATEGORIES] = {
	1, 16, 24, 14, 16, 12, 16, 14, 5
};

static int gCategoryColorSets[NUM_ZOMBATAR_CATEGORIES] = {
	0, 1, 1, 2, 2, 0, 0, 0, 0
};

static Image** GetCategoryImages(int category)
{
	switch (category)
	{
	case ZOMBATAR_CATEGORY_HAIR:        return GetHairImages();
	case ZOMBATAR_CATEGORY_FACIAL_HAIR: return GetFacialHairImages();
	case ZOMBATAR_CATEGORY_HATS:        return GetHatsImages();
	case ZOMBATAR_CATEGORY_EYEWEAR:     return GetEyewearImages();
	case ZOMBATAR_CATEGORY_CLOTHES:     return GetClothesImages();
	case ZOMBATAR_CATEGORY_ACCESSORIES: return GetAccessoryImages();
	case ZOMBATAR_CATEGORY_TIDBITS:     return GetTidbitsImages();
	case ZOMBATAR_CATEGORY_BACKDROPS:   return GetBackdropImages();
	default: return nullptr;
	}
}

static Image** GetCategoryMasks(int category)
{
	switch (category)
	{
	case ZOMBATAR_CATEGORY_HAIR:        return GetHairMasks();
	case ZOMBATAR_CATEGORY_FACIAL_HAIR: return GetFacialHairMasks();
	case ZOMBATAR_CATEGORY_HATS:        return GetHatsMasks();
	case ZOMBATAR_CATEGORY_EYEWEAR:     return GetEyewearMasks();
	default: return nullptr;
	}
}

// Color palettes matching original GOTY Zombatar
static Color gSkinColors[12] = {
	Color(0xF5, 0xD0, 0xA9), Color(0xE8, 0xC0, 0x98), Color(0xD4, 0xA8, 0x80),
	Color(0xC2, 0x90, 0x70), Color(0xB0, 0x80, 0x60), Color(0xA0, 0x70, 0x55),
	Color(0x8E, 0x60, 0x48), Color(0x7A, 0x50, 0x40), Color(0x68, 0x42, 0x35),
	Color(0x50, 0x32, 0x28), Color(0x3A, 0x24, 0x1C), Color(0x28, 0x18, 0x12)
};

static Color gHairColors[30] = {
	Color(0x00, 0x00, 0x00), Color(0x1A, 0x1A, 0x1A), Color(0x33, 0x33, 0x33),
	Color(0x4D, 0x4D, 0x4D), Color(0x66, 0x66, 0x66), Color(0x80, 0x80, 0x80),
	Color(0x99, 0x99, 0x99), Color(0xB3, 0xB3, 0xB3), Color(0xCC, 0xCC, 0xCC),
	Color(0xE6, 0xE6, 0xE6), Color(0xFF, 0xFF, 0xFF), Color(0x33, 0x11, 0x00),
	Color(0x55, 0x22, 0x00), Color(0x77, 0x33, 0x00), Color(0x99, 0x44, 0x00),
	Color(0xBB, 0x55, 0x00), Color(0xDD, 0x66, 0x00), Color(0xFF, 0x77, 0x00),
	Color(0xFF, 0x99, 0x33), Color(0xFF, 0xBB, 0x66), Color(0xFF, 0xDD, 0x99),
	Color(0x88, 0x00, 0x00), Color(0xAA, 0x11, 0x11), Color(0xCC, 0x22, 0x22),
	Color(0xDD, 0x44, 0x44), Color(0xEE, 0x66, 0x66), Color(0x44, 0x00, 0x88),
	Color(0x66, 0x22, 0xAA), Color(0x88, 0x44, 0xCC), Color(0xAA, 0x66, 0xEE)
};

static Color gHatEyewearColors[49] = {
	Color(0xFF, 0x00, 0x00), Color(0xCC, 0x00, 0x00), Color(0x99, 0x00, 0x00),
	Color(0x66, 0x00, 0x00), Color(0xFF, 0x66, 0x66), Color(0xFF, 0x99, 0x99),
	Color(0xFF, 0x80, 0x00), Color(0xCC, 0x66, 0x00), Color(0x99, 0x4D, 0x00),
	Color(0xFF, 0xB3, 0x66), Color(0xFF, 0xCC, 0x99), Color(0xFF, 0xFF, 0x00),
	Color(0xCC, 0xCC, 0x00), Color(0x99, 0x99, 0x00), Color(0x66, 0x66, 0x00),
	Color(0xFF, 0xFF, 0x66), Color(0xFF, 0xFF, 0x99), Color(0x00, 0xFF, 0x00),
	Color(0x00, 0xCC, 0x00), Color(0x00, 0x99, 0x00), Color(0x00, 0x66, 0x00),
	Color(0x66, 0xFF, 0x66), Color(0x99, 0xFF, 0x99), Color(0x00, 0xFF, 0xFF),
	Color(0x00, 0xCC, 0xCC), Color(0x00, 0x99, 0x99), Color(0x00, 0x66, 0x66),
	Color(0x66, 0xFF, 0xFF), Color(0x99, 0xFF, 0xFF), Color(0x00, 0x00, 0xFF),
	Color(0x00, 0x00, 0xCC), Color(0x00, 0x00, 0x99), Color(0x00, 0x00, 0x66),
	Color(0x66, 0x66, 0xFF), Color(0x99, 0x99, 0xFF), Color(0xFF, 0x00, 0xFF),
	Color(0xCC, 0x00, 0xCC), Color(0x99, 0x00, 0x99), Color(0x66, 0x00, 0x66),
	Color(0xFF, 0x66, 0xFF), Color(0xFF, 0x99, 0xFF), Color(0x80, 0x00, 0x80),
	Color(0x40, 0x00, 0x40), Color(0xFF, 0xFF, 0xFF), Color(0xCC, 0xCC, 0xCC),
	Color(0x99, 0x99, 0x99), Color(0x66, 0x66, 0x66), Color(0x33, 0x33, 0x33),
	Color(0x00, 0x00, 0x00)
};

static int gColorSetCounts[3] = { 12, 30, 49 };

static constexpr int CATEGORY_BTN_X = 33;
static constexpr int CATEGORY_BTN_Y0 = 103;
static constexpr int CATEGORY_BTN_W = 104;
static constexpr int CATEGORY_BTN_H = 39;
static constexpr int CATEGORY_FIRST_GAP = 100;
static constexpr int CATEGORY_OVERLAP = 3;

static constexpr int GRID_X = 195;
static constexpr int GRID_Y = 103;
static constexpr int GRID_CELL = 110;
static constexpr int GRID_SPACING = 120;
static constexpr int GRID_COLS = 3;
static constexpr int GRID_SLOTS = 9;

static constexpr int BACK_BTN_X = 23;
static constexpr int BACK_BTN_Y = 528;

static constexpr int FINISHED_BTN_X = 667;
static constexpr int FINISHED_BTN_Y = 528;

static constexpr int NEW_ZOMBIE_BTN_X = 23;
static constexpr int NEW_ZOMBIE_BTN_Y = 500;

static constexpr int PREV_BTN_X = 195;
static constexpr int PREV_BTN_Y = 540;

static constexpr int NEXT_BTN_X = 575;
static constexpr int NEXT_BTN_Y = 540;

static constexpr int PREVIEW_X = 600;
static constexpr int PREVIEW_Y = 80;

static constexpr int COLOR_BG_X = 195;
static constexpr int COLOR_BG_Y = 130;
static constexpr int COLOR_GRID_X = 217;
static constexpr int COLOR_GRID_Y = 150;
static constexpr int COLOR_CELL = 20;
static constexpr int COLOR_SPACING = 30;
static constexpr int COLOR_COLS = 6;

static constexpr int SLIDE_DURATION = 30;
static constexpr int SLIDE_OFFSET = 360;

ZombatarWidget::ZombatarWidget(LawnApp* theApp)
{
	mApp = theApp;
	mWidth = theApp->mWidth;
	mHeight = theApp->mHeight;
	mState = State::MAIN;
	mCurrentCategory = ZOMBATAR_CATEGORY_SKIN;
	mCurrentPage = 0;
	mSlideCounter = 0;
	mSlideStartX = 0;
	mSlideTargetX = 0;
	mCurrentHeadIndex = -1;
	mNeedToSave = false;
	mShowingNewZombie = true;
	memset(&mCurrentHead, 0xFF, sizeof(ZombatarHead));

	mLoadedResourceNames.push_back("DelayLoad_Zombatar");
	for (std::string& aResource : mLoadedResourceNames)
		TodLoadResources(aResource.c_str());

	mBackButton = MakeNewButton(Zombatar_Back, this, "", nullptr,
		IMAGE_ZOMBATAR_BACK_BUTTON, IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT,
		IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT);
	mBackButton->Resize(BACK_BTN_X, BACK_BTN_Y, 98, 26);

	mFinishedButton = MakeNewButton(Zombatar_Finished, this, "", nullptr,
		IMAGE_ZOMBATAR_FINISHED_BUTTON, IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT,
		IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT);
	mFinishedButton->Resize(FINISHED_BTN_X, FINISHED_BTN_Y, 103, 26);

	mNewZombieButton = MakeNewButton(Zombatar_NewZombie, this, "", nullptr,
		IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON, IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON_HIGHLIGHT,
		IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON_HIGHLIGHT);
	mNewZombieButton->Resize(NEW_ZOMBIE_BTN_X, NEW_ZOMBIE_BTN_Y, 234, 39);

	mDeleteButton = MakeNewButton(Zombatar_Delete, this, "", nullptr,
		IMAGE_ZOMBATAR_ACCEPT_BUTTON, IMAGE_ZOMBATAR_ACCEPT_BUTTON_HIGHLIGHT,
		IMAGE_ZOMBATAR_ACCEPT_BUTTON_HIGHLIGHT);
	mDeleteButton->Resize(270, 500, 98, 26);

	mPrevButton = MakeNewButton(Zombatar_PrevPage, this, "", nullptr,
		IMAGE_ZOMBATAR_PREV_BUTTON, IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT,
		IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT);
	mPrevButton->Resize(PREV_BTN_X, PREV_BTN_Y, 33, 38);

	mNextButton = MakeNewButton(Zombatar_NextPage, this, "", nullptr,
		IMAGE_ZOMBATAR_NEXT_BUTTON, IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT,
		IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT);
	mNextButton->Resize(NEXT_BTN_X, NEXT_BTN_Y, 33, 38);

	int aCatY = CATEGORY_BTN_Y0;
	for (int aCategory = 0; aCategory < NUM_ZOMBATAR_CATEGORIES; aCategory++)
	{
		Image* aOverImage = GetCategoryButtonOver(aCategory);
		if (aOverImage == nullptr)
			aOverImage = GetCategoryButtonHighlight(aCategory);

		mCategoryButtons[aCategory] = MakeNewButton(
			Zombatar_CategoryBase + aCategory, this, "", nullptr,
			GetCategoryButtonImage(aCategory), GetCategoryButtonHighlight(aCategory), aOverImage);
		mCategoryButtons[aCategory]->Resize(CATEGORY_BTN_X, aCatY, CATEGORY_BTN_W, CATEGORY_BTN_H);

		if (aCategory == 0)
			aCatY += CATEGORY_BTN_H + CATEGORY_FIRST_GAP;
		else
			aCatY += CATEGORY_BTN_H - CATEGORY_OVERLAP;
	}

	for (int aSlot = 0; aSlot < GRID_SLOTS; aSlot++)
	{
		mAccessoryButtons[aSlot] = new ButtonWidget(Zombatar_AccessoryBase + aSlot, this);
		mAccessoryButtons[aSlot]->mDoFinger = true;
		mAccessoryButtons[aSlot]->Resize(
			GRID_X + (aSlot % GRID_COLS) * GRID_SPACING,
			GRID_Y + (aSlot / GRID_COLS) * GRID_SPACING,
			GRID_CELL, GRID_CELL);
	}
}

ZombatarWidget::~ZombatarWidget()
{
	delete mBackButton;
	delete mFinishedButton;
	delete mNewZombieButton;
	delete mDeleteButton;
	delete mPrevButton;
	delete mNextButton;

	for (int i = 0; i < NUM_ZOMBATAR_CATEGORIES; i++)
		delete mCategoryButtons[i];

	for (int i = 0; i < 9; i++)
		delete mAccessoryButtons[i];
}

void ZombatarWidget::Update()
{
	Widget::Update();

	if (mSlideCounter > 0)
	{
		mSlideCounter--;
		if (mSlideCounter == 0)
			mState = State::MAIN;
	}

	MarkDirty();
}

void ZombatarWidget::Draw(Graphics* g)
{
	g->SetLinearBlend(true);

	g->DrawImage(IMAGE_ZOMBATAR_MAIN_BG, 0, 0);
	g->DrawImage(IMAGE_ZOMBATAR_WIDGET_BG, 0, 0);
	g->DrawImage(IMAGE_ZOMBATAR_WIDGET_INNER_BG, 127, 100);

	DrawPreview(g);
	DrawCategoryButtons(g);
	DrawAccessoryGrid(g);
	DrawActionButtons(g);

	if (IMAGE_ZOMBATAR_DISPLAY_WINDOW)
		g->DrawImage(IMAGE_ZOMBATAR_DISPLAY_WINDOW, 5, 0);

	if (mState == State::COLOR_PICKER)
		DrawColorPicker(g);
}

void ZombatarWidget::DrawPreview(Graphics* theG)
{
	if (!mShowingNewZombie)
		return;

	Image* aHeadImage = mApp->mZombatarComposer->GetHeadImage(mCurrentHead);
	if (aHeadImage)
		theG->DrawImage(aHeadImage, PREVIEW_X, PREVIEW_Y);
}

void ZombatarWidget::DrawCategoryButtons(Graphics* theG)
{
	for (int i = 0; i < NUM_ZOMBATAR_CATEGORIES; i++)
	{
		bool isSelected = (mCurrentCategory == i);
		if (isSelected)
		{
			theG->SetColorizeImages(true);
			theG->SetColor(Color(200, 200, 200));
		}
		mCategoryButtons[i]->Draw(theG);
		if (isSelected)
			theG->SetColorizeImages(false);
	}
}

void ZombatarWidget::DrawAccessoryGrid(Graphics* theG)
{
	int aItemCount = gCategoryItemCounts[mCurrentCategory];
	int aItemsPerPage = 9;
	int aStartIdx = mCurrentPage * aItemsPerPage;
	int aEndIdx = std::min(aStartIdx + aItemsPerPage, aItemCount);
	int aOffsetX = 0;

	if (mState == State::PREV_ANIM)
	{
		aOffsetX = TodAnimateCurve(SLIDE_DURATION, 0, mSlideCounter, -SLIDE_OFFSET, 0, TodCurves::CURVE_EASE_IN_OUT);
	}
	else if (mState == State::NEXT_ANIM)
	{
		aOffsetX = TodAnimateCurve(SLIDE_DURATION, 0, mSlideCounter, SLIDE_OFFSET, 0, TodCurves::CURVE_EASE_IN_OUT);
	}

	for (int i = aStartIdx; i < aEndIdx; i++)
	{
		int aLocalIdx = i - aStartIdx;
		int aX = GRID_X + (aLocalIdx % GRID_COLS) * GRID_SPACING + aOffsetX;
		int aY = GRID_Y + (aLocalIdx / GRID_COLS) * GRID_SPACING;

		bool isSelected = false;
		switch (mCurrentCategory)
		{
		case ZOMBATAR_CATEGORY_SKIN:
			isSelected = (mCurrentHead.mSkinColor == static_cast<uint32_t>(i));
			break;
		case ZOMBATAR_CATEGORY_HAIR:
			isSelected = (mCurrentHead.mHairType == static_cast<uint32_t>(i));
			break;
		case ZOMBATAR_CATEGORY_FACIAL_HAIR:
			isSelected = (mCurrentHead.mFacialHairType == static_cast<uint32_t>(i));
			break;
		case ZOMBATAR_CATEGORY_HATS:
			isSelected = (mCurrentHead.mHatType == static_cast<uint32_t>(i));
			break;
		case ZOMBATAR_CATEGORY_EYEWEAR:
			isSelected = (mCurrentHead.mEyewearType == static_cast<uint32_t>(i));
			break;
		case ZOMBATAR_CATEGORY_CLOTHES:
			isSelected = (mCurrentHead.mClothesType == static_cast<uint32_t>(i));
			break;
		case ZOMBATAR_CATEGORY_ACCESSORIES:
			isSelected = (mCurrentHead.mAccessoryType == static_cast<uint32_t>(i));
			break;
		case ZOMBATAR_CATEGORY_TIDBITS:
			isSelected = (mCurrentHead.mTidbitsType == static_cast<uint32_t>(i));
			break;
		case ZOMBATAR_CATEGORY_BACKDROPS:
			isSelected = (mCurrentHead.mBackdropType == static_cast<uint32_t>(i));
			break;
		default:
			break;
		}

		theG->DrawImage(isSelected ? IMAGE_ZOMBATAR_ACCESSORY_BG_HIGHLIGHT : IMAGE_ZOMBATAR_ACCESSORY_BG, aX, aY);

		Image* aItemImage = GetCategoryImage(mCurrentCategory, i);
		if (aItemImage)
			theG->DrawImage(aItemImage, aX + 5, aY + 5);
	}
}

void ZombatarWidget::DrawActionButtons(Graphics* theG)
{
	mBackButton->Draw(theG);
	mFinishedButton->Draw(theG);

	if (mCurrentHeadIndex >= 0)
	{
		mNewZombieButton->Draw(theG);
		mDeleteButton->Draw(theG);
	}

	int aTotalPages = GetTotalPages();
	if (aTotalPages > 1)
	{
		if (mCurrentPage > 0)
			mPrevButton->Draw(theG);
		if (mCurrentPage < aTotalPages - 1)
			mNextButton->Draw(theG);
	}
}

void ZombatarWidget::DrawColorPicker(Graphics* theG)
{
	theG->DrawImage(IMAGE_ZOMBATAR_COLORS_BG, COLOR_BG_X, COLOR_BG_Y);

	int aColorSet = gCategoryColorSets[mCurrentCategory];
	int aColorCount = gColorSetCounts[aColorSet];
	for (int i = 0; i < aColorCount; i++)
	{
		int aX = COLOR_GRID_X + (i % COLOR_COLS) * COLOR_SPACING;
		int aY = COLOR_GRID_Y + (i / COLOR_COLS) * COLOR_SPACING;
		theG->SetColor(GetCategoryColor(mCurrentCategory, i));
		theG->FillRect(aX, aY, COLOR_CELL, COLOR_CELL);
		theG->SetColor(Color(0, 0, 0));
		theG->DrawRect(aX, aY, COLOR_CELL, COLOR_CELL);
	}
}

void ZombatarWidget::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);

	AddWidget(mBackButton);
	AddWidget(mFinishedButton);
	AddWidget(mNewZombieButton);
	AddWidget(mDeleteButton);
	AddWidget(mPrevButton);
	AddWidget(mNextButton);

	for (int i = 0; i < NUM_ZOMBATAR_CATEGORIES; i++)
		AddWidget(mCategoryButtons[i]);

	for (int i = 0; i < 9; i++)
		AddWidget(mAccessoryButtons[i]);
}

void ZombatarWidget::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);

	RemoveWidget(mBackButton);
	RemoveWidget(mFinishedButton);
	RemoveWidget(mNewZombieButton);
	RemoveWidget(mDeleteButton);
	RemoveWidget(mPrevButton);
	RemoveWidget(mNextButton);

	for (int i = 0; i < NUM_ZOMBATAR_CATEGORIES; i++)
		RemoveWidget(mCategoryButtons[i]);

	for (int i = 0; i < 9; i++)
		RemoveWidget(mAccessoryButtons[i]);
}

void ZombatarWidget::OrderInManagerChanged()
{
	Widget::OrderInManagerChanged();

	mWidgetManager->PutInfront(mBackButton, this);
	mWidgetManager->PutInfront(mFinishedButton, this);
	mWidgetManager->PutInfront(mNewZombieButton, this);
	mWidgetManager->PutInfront(mDeleteButton, this);
	mWidgetManager->PutInfront(mPrevButton, this);
	mWidgetManager->PutInfront(mNextButton, this);

	for (int i = 0; i < NUM_ZOMBATAR_CATEGORIES; i++)
		mWidgetManager->PutInfront(mCategoryButtons[i], this);

	for (int i = 0; i < 9; i++)
		mWidgetManager->PutInfront(mAccessoryButtons[i], this);
}

void ZombatarWidget::ButtonPress(int theId)
{
	mApp->PlaySample(Sexy::SOUND_BUTTONCLICK);
}

void ZombatarWidget::ButtonDepress(int theId)
{
	if (theId == Zombatar_Back)
	{
		if (mNeedToSave)
			SaveCurrentHead();
		mApp->mGameSelector->SlideTo(0, 0);
		mApp->mGameSelector->mWidgetManager->SetFocus(mApp->mGameSelector);
		return;
	}

	if (theId == Zombatar_Finished)
	{
		if (mNeedToSave)
			SaveCurrentHead();
		mApp->mGameSelector->SlideTo(0, 0);
		mApp->mGameSelector->mWidgetManager->SetFocus(mApp->mGameSelector);
		return;
	}

	if (theId == Zombatar_NewZombie)
	{
		if (mNeedToSave && mCurrentHeadIndex >= 0)
		{
			int aResult = mApp->LawnMessageBox(
				Dialogs::DIALOG_MESSAGE,
				"[ZOMBATAR_FINISHED_WARNING_HEADER]",
				"[ZOMBATAR_FINISHED_WARNING_TEXT]",
				"[DIALOG_BUTTON_YES]",
				"[DIALOG_BUTTON_NO]",
				Dialog::BUTTONS_YES_NO);
			if (aResult == Dialog::ID_YES)
				SaveCurrentHead();
		}
		memset(&mCurrentHead, 0xFF, sizeof(ZombatarHead));
		mCurrentHead.mSkinColor = 0;
		mCurrentHeadIndex = -1;
		mNeedToSave = true;
		return;
	}

	if (theId == Zombatar_Delete)
	{
		if (mCurrentHeadIndex >= 0)
		{
			int aResult = mApp->LawnMessageBox(
				Dialogs::DIALOG_MESSAGE,
				"[ZOMBATAR_DELETE_HEADER]",
				"[ZOMBATAR_DELETE_BODY]",
				"[DIALOG_BUTTON_YES]",
				"[DIALOG_BUTTON_NO]",
				Dialog::BUTTONS_YES_NO);
			if (aResult == Dialog::ID_YES)
			{
				mApp->mPlayerInfo->DeleteZombatarHead(mCurrentHeadIndex);
				mApp->mPlayerInfo->SaveDetails();
				memset(&mCurrentHead, 0xFF, sizeof(ZombatarHead));
				mCurrentHead.mSkinColor = 0;
				mCurrentHeadIndex = -1;
				mNeedToSave = true;
			}
		}
		return;
	}

	if (theId == Zombatar_PrevPage)
	{
		if (mCurrentPage > 0)
		{
			mCurrentPage--;
			mState = State::PREV_ANIM;
			mSlideCounter = SLIDE_DURATION;
		}
		return;
	}

	if (theId == Zombatar_NextPage)
	{
		int aTotalPages = GetTotalPages();
		if (mCurrentPage < aTotalPages - 1)
		{
			mCurrentPage++;
			mState = State::NEXT_ANIM;
			mSlideCounter = SLIDE_DURATION;
		}
		return;
	}

	if (theId >= Zombatar_CategoryBase && theId < static_cast<int>(Zombatar_CategoryBase) + NUM_ZOMBATAR_CATEGORIES)
	{
		mCurrentCategory = theId - Zombatar_CategoryBase;
		mCurrentPage = 0;
		return;
	}

	if (theId >= Zombatar_AccessoryBase && theId < Zombatar_AccessoryBase + 9)
	{
		int aItemIdx = mCurrentPage * 9 + (theId - Zombatar_AccessoryBase);
		if (aItemIdx >= gCategoryItemCounts[mCurrentCategory])
			return;

		uint32_t aValue = static_cast<uint32_t>(aItemIdx);
		bool isAlreadySelected = false;
		if (mCurrentCategory == ZOMBATAR_CATEGORY_SKIN)
		{
			isAlreadySelected = (mCurrentHead.mSkinColor == aValue);
			mCurrentHead.mSkinColor = aValue;
		}
		else if (mCurrentCategory == ZOMBATAR_CATEGORY_HAIR)
		{
			isAlreadySelected = (mCurrentHead.mHairType == aValue);
			mCurrentHead.mHairType = aValue;
			if (gCategoryColorSets[mCurrentCategory] > 0 && mCurrentHead.mHairColor == 0x2F)
				mCurrentHead.mHairColor = 0x0C;
		}
		else if (mCurrentCategory == ZOMBATAR_CATEGORY_FACIAL_HAIR)
		{
			isAlreadySelected = (mCurrentHead.mFacialHairType == aValue);
			mCurrentHead.mFacialHairType = aValue;
			if (gCategoryColorSets[mCurrentCategory] > 0 && mCurrentHead.mFacialHairColor == 0x2F)
				mCurrentHead.mFacialHairColor = 0x0C;
		}
		else if (mCurrentCategory == ZOMBATAR_CATEGORY_HATS)
		{
			isAlreadySelected = (mCurrentHead.mHatType == aValue);
			mCurrentHead.mHatType = aValue;
			if (gCategoryColorSets[mCurrentCategory] > 0 && mCurrentHead.mHatColor == 0x2F)
				mCurrentHead.mHatColor = 0x1E;
		}
		else if (mCurrentCategory == ZOMBATAR_CATEGORY_EYEWEAR)
		{
			isAlreadySelected = (mCurrentHead.mEyewearType == aValue);
			mCurrentHead.mEyewearType = aValue;
			if (gCategoryColorSets[mCurrentCategory] > 0 && mCurrentHead.mEyewearColor == 0x2F)
				mCurrentHead.mEyewearColor = 0x1E;
		}
		else if (mCurrentCategory == ZOMBATAR_CATEGORY_CLOTHES)
		{
			isAlreadySelected = (mCurrentHead.mClothesType == aValue);
			mCurrentHead.mClothesType = aValue;
		}
		else if (mCurrentCategory == ZOMBATAR_CATEGORY_ACCESSORIES)
		{
			isAlreadySelected = (mCurrentHead.mAccessoryType == aValue);
			mCurrentHead.mAccessoryType = aValue;
		}
		else if (mCurrentCategory == ZOMBATAR_CATEGORY_TIDBITS)
		{
			isAlreadySelected = (mCurrentHead.mTidbitsType == aValue);
			mCurrentHead.mTidbitsType = aValue;
		}
		else if (mCurrentCategory == ZOMBATAR_CATEGORY_BACKDROPS)
		{
			isAlreadySelected = (mCurrentHead.mBackdropType == aValue);
			mCurrentHead.mBackdropType = aValue;
		}

		if (isAlreadySelected && (gCategoryColorSets[mCurrentCategory] > 0 || mCurrentCategory == ZOMBATAR_CATEGORY_SKIN))
		{
			mState = State::COLOR_PICKER;
		}
		else
		{
			mState = State::MAIN;
		}
		mNeedToSave = true;
		return;
	}
}

void ZombatarWidget::MouseDown(int theX, int theY, int theClickCount)
{
	(void)theClickCount;

	if (mState == State::COLOR_PICKER)
	{
		int aColorSet = gCategoryColorSets[mCurrentCategory];
		int aColorCount = gColorSetCounts[aColorSet];
		for (int i = 0; i < aColorCount; i++)
		{
			int aX = COLOR_GRID_X + (i % COLOR_COLS) * COLOR_SPACING;
			int aY = COLOR_GRID_Y + (i / COLOR_COLS) * COLOR_SPACING;
			if (theX >= aX && theX < aX + COLOR_CELL && theY >= aY && theY < aY + COLOR_CELL)
			{
				if (mCurrentCategory == ZOMBATAR_CATEGORY_SKIN)
					mCurrentHead.mSkinColor = i;
				else if (mCurrentCategory == ZOMBATAR_CATEGORY_HAIR)
					mCurrentHead.mHairColor = i;
				else if (mCurrentCategory == ZOMBATAR_CATEGORY_FACIAL_HAIR)
					mCurrentHead.mFacialHairColor = i;
				else if (mCurrentCategory == ZOMBATAR_CATEGORY_HATS)
					mCurrentHead.mHatColor = i;
				else if (mCurrentCategory == ZOMBATAR_CATEGORY_EYEWEAR)
					mCurrentHead.mEyewearColor = i;

				mNeedToSave = true;
				mState = State::MAIN;
				mApp->PlaySample(Sexy::SOUND_BUTTONCLICK);
				return;
			}
		}

		// Click outside color picker closes it
		mState = State::MAIN;
	}
}

void ZombatarWidget::MouseUp(int theX, int theY, int theClickCount)
{
	(void)theX;
	(void)theY;
	(void)theClickCount;
}

void ZombatarWidget::EnterEditor()
{
	mState = State::MAIN;
	mCurrentCategory = ZOMBATAR_CATEGORY_SKIN;
	mCurrentPage = 0;
	mSlideCounter = 0;
	mNeedToSave = false;

	if (mApp->mPlayerInfo->mZombatarHeadCount > 0 && mCurrentHeadIndex >= 0)
	{
		LoadCurrentHead();
	}
	else
	{
		memset(&mCurrentHead, 0xFF, sizeof(ZombatarHead));
		mCurrentHead.mSkinColor = 0;
		mCurrentHeadIndex = -1;
	}
}

void ZombatarWidget::LoadCurrentHead()
{
	const ZombatarHead* head = mApp->mPlayerInfo->GetZombatarHead(mCurrentHeadIndex);
	if (head)
		mCurrentHead = *head;
}

void ZombatarWidget::SaveCurrentHead()
{
	if (mCurrentHeadIndex >= 0)
	{
		ZombatarHead* head = mApp->mPlayerInfo->GetZombatarHeadWritable(mCurrentHeadIndex);
		if (head)
			*head = mCurrentHead;
	}
	else
	{
		mCurrentHeadIndex = mApp->mPlayerInfo->AddZombatarHead(mCurrentHead);
	}
	mApp->mPlayerInfo->SaveDetails();
	mNeedToSave = false;
}

int ZombatarWidget::GetTotalPages() const
{
	int aItemCount = gCategoryItemCounts[mCurrentCategory];
	return (aItemCount + 8) / 9;
}

Image* ZombatarWidget::GetCategoryImage(int theCategory, int theItemIndex) const
{
	if (theCategory < 0 || theCategory >= NUM_ZOMBATAR_CATEGORIES)
		return nullptr;
	if (theItemIndex < 0 || theItemIndex >= gCategoryItemCounts[theCategory])
		return nullptr;
	Image** aImages = GetCategoryImages(theCategory);
	if (!aImages)
		return nullptr;
	return aImages[theItemIndex];
}

Image* ZombatarWidget::GetCategoryMask(int theCategory, int theItemIndex) const
{
	if (theCategory < 0 || theCategory >= NUM_ZOMBATAR_CATEGORIES)
		return nullptr;
	if (theItemIndex < 0 || theItemIndex >= gCategoryItemCounts[theCategory])
		return nullptr;
	Image** aMasks = GetCategoryMasks(theCategory);
	if (!aMasks)
		return nullptr;
	return aMasks[theItemIndex];
}

Color ZombatarWidget::GetCategoryColor(int theCategory, int theColorIndex) const
{
	int aColorSet = gCategoryColorSets[theCategory];
	int aCount = gColorSetCounts[aColorSet];
	if (theColorIndex < 0 || theColorIndex >= aCount)
		return Color::White;

	switch (aColorSet)
	{
	case 1:  return gHairColors[theColorIndex];
	case 2:  return gHatEyewearColors[theColorIndex];
	default: return gSkinColors[theColorIndex];
	}
}


