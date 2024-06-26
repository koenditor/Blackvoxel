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
 * ZGui_KeyChooser.h
 *
 *  Created on: 15 juin 2011
 *      Author: laurent
 */

#pragma once 

#include <SDL2/SDL_scancode.h>

#include "z/ZString.h"
#include "ZGui.h"
#include "ZGui_FontFrame.h"
#include "z/ZTypes.h"

class ZTileSet;
class ZTileStyle;

class ZFrame_KeyChooser : public ZFrame
{
  protected:
    ZTileStyle * FontTileStyle;
    ZTileSet   * GUITileSet;
    SDL_Scancode     Actual_KeySet;
    ZString    DisplayText;
  public:
    // ZTileFrame       BackTile;
    ZFrame_FontFrame Text;


    // Events

    bool Event_KeyChanged;

    ZFrame_KeyChooser()
    {
      FrameType = MulticharConst('K','C','H','O');
      FontTileStyle = nullptr;
      GUITileSet = nullptr;
      //BackTile.SetTile(6);
      Actual_KeySet = SDL_SCANCODE_A;
      Text.SetDisplayText("A");
      Flag_Show_Frame = false;
      Flag_Enable_Proc_GetFocus = true;
      Flag_Enable_Proc_KeyDown = true;
      Event_KeyChanged = 0;
    }

    virtual void SetFontTileStyle( ZTileStyle * TileStyle )
    {
      FontTileStyle = TileStyle;
      Text.SetStyle(TileStyle);
    }

    virtual void SetGUITileset( ZTileSet   * TileSet   )
    {
      GUITileSet = TileSet;
      // BackTile.SetTileSet(TileSet);
    }

    virtual void AddedToFrameCallback(ZFrame * Frame)
    {
      // AddFrame(&BackTile);
      AddFrame(&Text);
    };

    virtual void GetTextDisplaySize(ZVector2f * OutSize)
    {
      Text.SetDisplayText("(HIT NEW KEY) ");
      Text.GetTextDisplaySize(OutSize);
      Text.SetDisplayText(DisplayText.String);
    }

    virtual void SetSize    (float Width, float Height)
    {
      // BackTile.SetPosition(0.0f,0.0f);
      Text.SetPosition(0.0f,0.0f);
      // BackTile.SetSize(Width,Height);
      Text.SetSize(Width+100.0f,Height);
      Dimensions.Width = Width ; Dimensions.Height = Height;
    }

    virtual void SetKey( SDL_Scancode Key);
    virtual SDL_Scancode GetKey() { return(Actual_KeySet); }


    virtual void GetFocus();
    virtual Bool KeyDown( SDL_Scancode KeySym );
    virtual void KeyChanged() {}
    bool Is_KeyChanged (bool Reset = true)     { bool res; res = Event_KeyChanged;  if (Reset) Event_KeyChanged = false;  return(res); }

};
