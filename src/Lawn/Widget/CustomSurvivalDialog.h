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

#ifndef __CUSTOMSURVIVALDIALOG_H__
#define __CUSTOMSURVIVALDIALOG_H__

#include "widget/Dialog.h"
#include "widget/CheckboxListener.h"

class LawnApp;
class LawnStoneButton;
class NewLawnButton;
namespace Sexy
{
	class Checkbox;
};

class CustomSurvivalDialog : public Sexy::Dialog, public Sexy::CheckboxListener
{
protected:
	enum
	{
		CustomSurvivalDialog_LevelUp,
		CustomSurvivalDialog_LevelDown,
		CustomSurvivalDialog_Boss,
		CustomSurvivalDialog_Zombotomy,
		CustomSurvivalDialog_Grave,
		CustomSurvivalDialog_Bungee,
		CustomSurvivalDialog_Fog,
		CustomSurvivalDialog_Cancel,
		CustomSurvivalDialog_Go,
	};

public:
	
	Dialog*		mNoD;
	LawnApp*				mApp;
	int						mCurrentLevel;
	LawnStoneButton*		mLevelUpButtom;
	LawnStoneButton*		mLevelDownButtom;
	Sexy::Checkbox*			mBossCheckbox;
	Sexy::Checkbox*			mZombotomyCheckbox;
	Sexy::Checkbox*			mGraveCheckbox;
	Sexy::Checkbox*			mBungeeCheckbox;
	Sexy::Checkbox*			mFogCheckbox;
	LawnStoneButton*		mCancelButton;
	LawnStoneButton*		mGoButton;
	int		 				mChallengeMode;

public:
	CustomSurvivalDialog(LawnApp* theApp, int theMode, Dialog* daNoD);
	~CustomSurvivalDialog();

	int						GetPreferredHeight(int theWidth);
	void					AddedToManager(Sexy::WidgetManager* theWidgetManager);
	void					RemovedFromManager(Sexy::WidgetManager* theWidgetManager);
	void					Resize(int theX, int theY, int theWidth, int theHeight);
	void					Draw(Sexy::Graphics* g);
	void					CheckboxChecked(int theId, bool checked);
	void					ButtonPress(int theId);
	void					ButtonDepress(int theId);
	void					KeyDown(Sexy::KeyCode theKey);
};

#endif
