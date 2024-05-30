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
 * ZScreen_SlotSelection.h
 *
 *  Created on: 30 mai 2011
 *      Author: laurent
 */

#pragma once 






#include "ZGui.h"

#include "ZGui_FontFrame.h"

#include "ZGame.h"

class ZScreen_SlotSelection : public ZScreen
{
  public:
    ZScreen_SlotSelection() { ChoosedSlot = 0; }
    enum {CHOICE_RETURN=0};
    ULong ChoosedSlot;
    virtual ULong ProcessScreen(ZGame * GameEnv);
};