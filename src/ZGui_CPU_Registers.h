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
 * ZGui_CPU_Registers.h
 *
 *  Created on: 21 déc. 2014
 *      Author: laurent
 */

#pragma once 

#include "z/ZString.h"
#include "ZGui.h"
#include "ZGui_FontFrame.h"
#include "z/ZTypes.h"

class ZVMachine;
class ZGame;

#define ZCPUREGISTERS_MAXLINECOUNT 32

class ZCPURegisters : public ZFrame
{
    ZGame * GameEnv;
    ZVMachine * VMachine;
    ZFrame_FontFrame Display[ZCPUREGISTERS_MAXLINECOUNT];
    ZString          DisplayText[ZCPUREGISTERS_MAXLINECOUNT];
    float Margin;
    ULong LineHeight;
    ULong DisplaySlots;
  public:

    ZCPURegisters ()
    {
      FrameType = MulticharConst('C','R','E','G');
      VMachine = nullptr;
      Margin = 2.0f;
      LineHeight = 12.0;
      GameEnv = nullptr;
      DisplaySlots = 0;
    }

    virtual void AddedToFrameCallback(ZFrame * Frame);

    virtual void GetEffectiveHeight(float & Height);

    void SetVMachine(ZVMachine * VMachine) {this->VMachine = VMachine;}

    void SetGameEnv(ZGame * GameEnv) { this->GameEnv = GameEnv; }

    virtual void SetSize    (float Width, float Height);

    virtual void Render(Frame_Dimensions * ParentPosition);

};
