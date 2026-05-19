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

#ifndef __ALLOWEDZOMBIEDIALOG_H__
#define __ALLOWEDZOMBIEDIALOG_H__

#include "widget/Dialog.h"

#define NUM_CHOOSABLE_ZOMBIES 32

class Plant;
class Zombie;
class LawnApp;
class GameButton;
class Reanimation;
class AllowedZombieDialog : public Dialog
{
private:
	enum
	{
		ALLOWEDZOMBIE_BUTTON_CLOSE = 0
	};

public:
	LawnApp*					mApp;
	Zombie*						mZombiePerfTest[400];
	LawnStoneButton*			mCloseButton;
	bool*						mZombieAllowed;
	int							mScrollY;
	
public:
	AllowedZombieDialog(LawnApp* theApp, bool* allowedZombies);
	~AllowedZombieDialog();

	void						AddedToManager(WidgetManager* theWidgetManager);
	void						RemovedFromManager(WidgetManager* theWidgetManager);
	void						Update();
	void						DrawZombies(Graphics* g);
	void						Draw(Graphics* g);
	void						Resize(int theX, int theY, int theWidth, int theHeight);
	/*inline*/ bool				ZombieHasSilhouette(ZombieType theZombieType);
	bool						ZombieIsShown(ZombieType theZombieType);
	bool						ZombieHasDescription(ZombieType theZombieType);
	void						GetZombiePosition(int num, int& x, int& y);
	int							ZombieHitTest(int x, int y);
	void						MouseUp(int x, int y, int theClickCount);
	void						MouseDown(int x, int y, int theClickCount);
    void            			MouseWheel(int theDelta);
	void						ButtonPress(int theId);
	void						ButtonDepress(int theId);
//	virtual void				KeyChar(char theChar);

	static ZombieType			GetZombieType(int theIndex);
	void 						ClearPlantsAndZombies();
};

#endif
