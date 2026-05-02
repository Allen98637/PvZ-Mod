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

#ifndef __ZOMBATARTOSDIALOG_H__
#define __ZOMBATARTOSDIALOG_H__

#include "LawnDialog.h"
#include "../../SexyAppFramework/widget/Slider.h"
#include "../../SexyAppFramework/widget/SliderListener.h"

class LawnApp;
class NewLawnButton;
using namespace Sexy;

class ZombatarTOSDialog : public LawnDialog, public SliderListener
{
public:
	ZombatarTOSDialog(LawnApp* theApp);
	virtual ~ZombatarTOSDialog();

	virtual void Draw(Graphics* g) override;
	virtual void MouseWheel(int theDelta) override;
	virtual void AddedToManager(WidgetManager* theWidgetManager) override;
	virtual void RemovedFromManager(WidgetManager* theWidgetManager) override;
	virtual void ButtonDepress(int theId) override;
	virtual void SliderVal(int theId, double theVal) override;

private:
	NewLawnButton*	mBackButton;
	NewLawnButton*	mAcceptButton;
	Slider*			mSlider;
	int				mScrollOffset;
	int				mTextAreaHeight;
	int				mTotalTextHeight;
};

#endif
