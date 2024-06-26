/*
 * This file is part of Blackvoxel.
 *
 * Copyright 2010-2014 Laurent Thiebaut & Olivia Merle
 *
 * Blackvoxel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Blackvoxel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * ZSettings_Hardware.h
 *
 *  Created on: 2 juin 2011
 *      Author: laurent
 */

#pragma once

#include "z/ZTypes.h"
#include "z/ZString.h"
#include "SDL2/SDL_scancode.h"

class ZHardwareInfo;

class ZSettings_Hardware
{
  public:
    // Resolution and screen
    int Setting_Resolution_h;
    int Setting_Resolution_v;
    ULong Setting_ViewPort_Offset_x;
    ULong Setting_ViewPort_Offset_y;
    ULong Setting_ViewPort_Size_x;
    ULong Setting_ViewPort_Size_y;
    bool  Setting_FullScreen;
    bool  Setting_NoframeWindow;
    bool  Setting_OldLooked;


    // Sound
    bool   Setting_SoundEnabled;
    double Setting_SoundVolume;

    // Mouse
    double Setting_MouseFactor;
    bool   Setting_MouseFlipY;

    // Keyboard
    SDL_Scancode Setting_Key_MoveForward;
    SDL_Scancode Setting_Key_MoveBackward;
    SDL_Scancode Setting_Key_MoveLeft;
    SDL_Scancode Setting_Key_MoveRight;
    SDL_Scancode Setting_Key_MoveUp;
    SDL_Scancode Setting_Key_MoveDown;
    SDL_Scancode Setting_Key_Jump;
    SDL_Scancode Setting_Key_Inventory;

    // Graphic quality;

    ULong Set_RenderDistance_Horiz;
    ULong Set_RenderDistance_Vert;
    ULong RenderingDistance_Horizontal;     // Computed depending on settings and machine.
    ULong RenderingDistance_Vertical;       // Computed depending on settings and machine.


    double Opt_SectCFactor; // Sector Rendering Culling Factor.
    double PixelAspectRatio;

    // Options for special gaming modes

    //bool Experimental_LearningMode;
    ZString Setting_Favorite_Editor;
    bool   RemoteExtension_Enable;
    UShort RemoteExtension_Port;

    // Setting Version.

    UShort Setting_Version;      // Actual Setting Version
    UShort File_Setting_Version; // Last loaded file Setting Version (when mistmatch with Setting_Version, some settings might have to be adjusted.).




    ZSettings_Hardware();
    bool Load();
    bool Save();
    void AdjustForRealHardware(ZHardwareInfo * HardwareInfo);

};
