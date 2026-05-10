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
#include "GameButton.h"
#include "../Cutscene.h"
#include "../LawnCommon.h"
#include "../../LawnApp.h"
#include "../System/Music.h"
#include "../../Resources.h"
#include "../../ConstEnums.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "widget/Checkbox.h"
#include "LawnDialog.h"
#include "CustomSurvivalDialog.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "graphics/ImageFont.h"

using namespace Sexy;

std::string levelList[6] = {"[DAY]", "[NIGHT]", "[POOL]", "[FOG]", "[ROOF]", "[NIGHT_ROOF]"};

CustomSurvivalDialog::CustomSurvivalDialog(LawnApp* theApp, int theMode, Dialog* daNoD) : 
	Dialog(nullptr, nullptr, Dialogs::DIALOG_CustomSurvival, true, "Custom Endless", "", "", Dialog::BUTTONS_NONE)
{
    mApp = theApp;
    mChallengeMode = theMode;
    mNoD = daNoD;
    SetColor(Dialog::COLOR_BUTTON_TEXT, Color(255, 255, 100));

    SetColor(0, { 0xE0,0xBB,0x62 });
    SetColor(1, { 0xE0,0xBB,0x62 });
    SetHeaderFont(Sexy::FONT_DWARVENTODCRAFT24);

    mWidth = 693;
    mHeight = 535;

    mCurrentLevel = 2;
    mLevelDownButtom = MakeButton(CustomSurvivalDialog::CustomSurvivalDialog_LevelDown, this, "<");
    mLevelUpButtom = MakeButton(CustomSurvivalDialog::CustomSurvivalDialog_LevelUp, this, ">");

    mBossCheckbox = MakeNewCheckbox(CustomSurvivalDialog::CustomSurvivalDialog_Boss, this, false);
    mZombotomyCheckbox = MakeNewCheckbox(CustomSurvivalDialog::CustomSurvivalDialog_Zombotomy, this, false);
    mGraveCheckbox = MakeNewCheckbox(CustomSurvivalDialog::CustomSurvivalDialog_Grave, this, false);
    mBungeeCheckbox = MakeNewCheckbox(CustomSurvivalDialog::CustomSurvivalDialog_Bungee, this, false);
    mFogCheckbox = MakeNewCheckbox(CustomSurvivalDialog::CustomSurvivalDialog_Fog, this, false);
    
    mCancelButton = MakeButton(CustomSurvivalDialog::CustomSurvivalDialog_Cancel, this, "[DIALOG_BUTTON_CANCEL]");
    mGoButton = MakeButton(CustomSurvivalDialog::CustomSurvivalDialog_Go, this, "[DIALOG_BUTTON_OK]");
}

CustomSurvivalDialog::~CustomSurvivalDialog()
{
    delete mLevelUpButtom;
    delete mLevelDownButtom;
    delete mBossCheckbox;
    delete mZombotomyCheckbox;
    delete mGraveCheckbox;
    delete mBungeeCheckbox;
    delete mFogCheckbox;
    delete mCancelButton;
    delete mGoButton;
}

int CustomSurvivalDialog::GetPreferredHeight(int theWidth)
{
    (void)theWidth;
    return IMAGE_OPTIONS_MENUBACK->mWidth;
}

void CustomSurvivalDialog::AddedToManager(Sexy::WidgetManager* theWidgetManager)
{
    Dialog::AddedToManager(theWidgetManager);
    AddWidget(mLevelUpButtom);
    AddWidget(mLevelDownButtom);
    AddWidget(mBossCheckbox);
    AddWidget(mZombotomyCheckbox);
    AddWidget(mGraveCheckbox);
    AddWidget(mBungeeCheckbox);
    AddWidget(mFogCheckbox);
    AddWidget(mCancelButton);
    AddWidget(mGoButton);
}

void CustomSurvivalDialog::RemovedFromManager(Sexy::WidgetManager* theWidgetManager)
{
    Dialog::RemovedFromManager(theWidgetManager);
    RemoveWidget(mLevelUpButtom);
    RemoveWidget(mLevelDownButtom);
    RemoveWidget(mBossCheckbox);
    RemoveWidget(mZombotomyCheckbox);
    RemoveWidget(mGraveCheckbox);
    RemoveWidget(mBungeeCheckbox);
    RemoveWidget(mFogCheckbox);
    RemoveWidget(mCancelButton);
    RemoveWidget(mGoButton);
}

void CustomSurvivalDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
    Dialog::Resize(theX, theY, theWidth, theHeight);
    mBossCheckbox->Resize(379, 205, 46, 45);
    mZombotomyCheckbox->Resize(380, 244, 46, 45);
    mGraveCheckbox->Resize(380, 285, 46, 45);
    mBungeeCheckbox->Resize(381, 324, 46, 45);
    mFogCheckbox->Resize(379, 365, 46, 45);
    mLevelDownButtom->Resize(64, 116, 71, 46);
    mLevelUpButtom->Resize(mWidth - 133, 118, 71, 46);

    mContentInsets = Insets(36, 35, 46, 36);
    int aButtonAreaX = mContentInsets.mLeft + mBackgroundInsets.mLeft - 5;
    int aButtonAreaY = mHeight - mContentInsets.mBottom - mBackgroundInsets.mBottom - IMAGE_BUTTON_LEFT->mHeight + 2;
    int aButtonAreaWidth = mWidth - mContentInsets.mRight - mBackgroundInsets.mRight - mBackgroundInsets.mLeft - mContentInsets.mLeft + 8;
    // 按钮贴图限制下，按钮的最小宽度
    int aButtonMinWidth = IMAGE_BUTTON_LEFT->mWidth + IMAGE_BUTTON_RIGHT->mWidth;
    // 按钮区域额外空余的宽度
    int aBtnMidWidth = IMAGE_BUTTON_MIDDLE->mWidth;
    int aButtonExtraWidth = (aButtonAreaWidth - 10) / 2 - aBtnMidWidth - aButtonMinWidth + 1;
    // 计算按钮可以拓展的额外宽度
    if (aButtonExtraWidth <= 0)
    {
        aButtonExtraWidth = 0;
    }
    else if (aBtnMidWidth > 0)
    {
        int anExtraWidth = aButtonExtraWidth % aBtnMidWidth;
        if (anExtraWidth)
        {
            // 不足中部贴图宽度的部分补充至中部贴图宽度
            aButtonExtraWidth += aBtnMidWidth - anExtraWidth;
        }
    }
    // 最终宽度
    int aButtonWidth = aButtonMinWidth + aButtonExtraWidth;
    aButtonAreaY -= 10;

    mCancelButton->Resize(aButtonAreaX, aButtonAreaY, aButtonWidth, IMAGE_BUTTON_LEFT->mHeight);
    mGoButton->Resize(aButtonAreaWidth - aButtonWidth + aButtonAreaX, aButtonAreaY, aButtonWidth, IMAGE_BUTTON_LEFT->mHeight);
}

void CustomSurvivalDialog::Draw(Sexy::Graphics* g)
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

    Sexy::Color aTextColor(107, 109, 145);
    Sexy::Color bTextColor(0xDD,0xDF,0xED);

    int aCheckboxLabelsX = 369;
    float aFontScale = static_cast<float>(mApp->GetDouble("OPTION_DLG_LABEL_FONT_SCALE", 1.0));
    if (aFontScale != 1.0f)
        g->SetScale(aFontScale, aFontScale, 0.0f, 0.0f);
    TodDrawString(g, levelList[mCurrentLevel], mWidth / 2, 140, FONT_DWARVENTODCRAFT18, bTextColor, DrawStringJustification::DS_ALIGN_CENTER);
    TodDrawString(g, "Boss Encounter", aCheckboxLabelsX, 227, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    TodDrawString(g, "Zombotomy", aCheckboxLabelsX, 267, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    TodDrawString(g, "Graves", aCheckboxLabelsX, 307, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    TodDrawString(g, "Bungee Wave", aCheckboxLabelsX, 347, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    TodDrawString(g, "Fog", aCheckboxLabelsX, 387, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    if (aFontScale != 1.0f)
        g->SetScale(1.0f, 1.0f, 0.0f, 0.0f);
}


void CustomSurvivalDialog::CheckboxChecked(int theId, bool checked)
{

}

void CustomSurvivalDialog::KeyDown(Sexy::KeyCode theKey)
{
    if (mApp->mBoard)
    {
        mApp->mBoard->DoTypingCheck(theKey);
    }

    /*if (theKey == KeyCode::KEYCODE_SPACE || theKey == KeyCode::KEYCODE_RETURN)
    {
        Dialog::ButtonDepress(CustomSurvivalDialog::CustomSurvivalDialog_Go);
    }
    else if (theKey == KeyCode::KEYCODE_ESCAPE)
    {
        Dialog::ButtonDepress(CustomSurvivalDialog::CustomSurvivalDialog_Cancel);
    }*/
}

void CustomSurvivalDialog::ButtonPress(int theId)
{
    (void)theId;
    mApp->PlaySample(SOUND_GRAVEBUTTON);
}

void CustomSurvivalDialog::ButtonDepress(int theId)
{
    Dialog::ButtonDepress(theId);

    switch (theId)
    {
    case CustomSurvivalDialog::CustomSurvivalDialog_LevelUp:
    {
        mCurrentLevel ++;
        if(mCurrentLevel >= 6) mCurrentLevel = 0;
        break;
    }

    case CustomSurvivalDialog::CustomSurvivalDialog_LevelDown:
    {
        mCurrentLevel --;
        if(mCurrentLevel <= 0) mCurrentLevel = 5;
        break;
    }

    case CustomSurvivalDialog::CustomSurvivalDialog_Cancel:
    {
        mApp->KillDialog(Dialogs::DIALOG_CustomSurvival);
        break;
    }

    case CustomSurvivalDialog::CustomSurvivalDialog_Go:
    {
        mApp->KillDialog(Dialogs::DIALOG_CustomSurvival);
        CustomSurvivalOption options = {
            static_cast<BackgroundType>(BACKGROUND_1_DAY + mCurrentLevel), mBossCheckbox->mChecked, mZombotomyCheckbox->mChecked,
            mGraveCheckbox->mChecked, mBungeeCheckbox->mChecked, mFogCheckbox->mChecked
        };
        if(mNoD){
            mApp->mMusic->StopAllMusic();
            mApp->mSoundSystem->CancelPausedFoley();
            mApp->KillNewOptionsDialog();
            mApp->KillDialog(Dialogs::DIALOG_CONTINUE);
            mApp->mBoardResult = BoardResult::BOARDRESULT_RESTART;
            mApp->mSawYeti = mApp->mBoard->mKilledYeti;
            mApp->PreNewGame(mApp->mGameMode, options);
        }
        else{
            mApp->KillChallengeScreen();
            mApp->PreNewGame((GameMode)(mChallengeMode + 1), options);
        }
        break;
    }
    }
}
