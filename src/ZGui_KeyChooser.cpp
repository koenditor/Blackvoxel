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
 * ZGui_KeyChooser.cpp
 *
 *  Created on: 15 juin 2011
 *      Author: laurent
 */

#include "ZGui_KeyChooser.h"

#include <SDL2/SDL_keyboard.h>



Bool ZFrame_KeyChooser::KeyDown( SDL_Scancode KeySym )
{
  ZVector2f Size;
  Actual_KeySet = KeySym;
  // DisplayText = (ULong)KeySym;
  DisplayText = SDL_GetScancodeName(KeySym);
  DisplayText.MakeUpper();
  Text.SetDisplayText(DisplayText.String);
  Text.GetTextDisplaySize(&Size);
  Text.SetSize(Size.x,Size.y);
  Text.SetPosition(Dimensions.Width / 2.0f - Size.x / 2.0f, Dimensions.Height / 2.0f - Size.y / 2.0f );

  Event_KeyChanged = true;
  KeyChanged();
  return(true);
}

void ZFrame_KeyChooser::SetKey( SDL_Scancode Key)
{
  ZVector2f Size;

  Actual_KeySet = Key;

  DisplayText = SDL_GetScancodeName(Key);
  DisplayText.MakeUpper();
  Text.SetDisplayText(DisplayText.String);
  Text.GetTextDisplaySize(&Size);
  Text.SetSize(Size.x,Size.y);
  Text.SetPosition(Dimensions.Width / 2.0f - Size.x / 2.0f, Dimensions.Height / 2.0f - Size.y / 2.0f );
}

void ZFrame_KeyChooser::GetFocus()
{
  ZVector2f Size;

  Text.SetDisplayText((char *)"(HIT NEW KEY)");
  Text.GetTextDisplaySize(&Size);
  Text.SetSize(Size.x,Size.y);
  Text.SetPosition(Dimensions.Width / 2.0f - Size.x / 2.0f, Dimensions.Height / 2.0f - Size.y / 2.0f );
}
