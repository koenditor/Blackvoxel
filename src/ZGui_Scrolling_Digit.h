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
 * ZGui_Scrolling_Digit.h
 *
 *  Created on: 10 févr. 2015
 *      Author: laurent
 */

#pragma once 

#include "z/ZString.h"

#include "ZGui.h"

#include "ZGui_FontFrame.h"

#include "ZGui_TileFrame.h"

class ZGame;

class ZFrame_ScrollingDigit : public ZFrame
{
    ZGame * GameEnv;

    ZFrame_FontFrame Digit;
    ZTileFrame Arrow_Up;
    ZTileFrame Arrow_Down;
    ZString    Text_Digit;
  public:

    ZMemSize * DigitAddress;
    ULong      DigitPosition;

    ZFrame_ScrollingDigit ()
    {
      FrameType = MulticharConst('S','D','G','T');
      GameEnv = 0;
      DigitAddress = 0;
      DigitPosition = 0;
    }

    void SetGameEnv(ZGame * GameEnv) { this->GameEnv = GameEnv; }
    void SetDigitAddress(ZMemSize * Address, ULong Position) {DigitAddress = Address; DigitPosition = Position; }

    virtual void AddedToFrameCallback(ZFrame * Frame);
    virtual void GetEffectiveSize(ZVector2f * Size);
    virtual void SetSize    (float Width, float Height);
    virtual void Render(Frame_Dimensions * ParentPosition);

    virtual Bool MouseButtonClick  (UShort nButton, Short Absolute_x, Short Absolute_y);

};
