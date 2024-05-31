/*
 * This file is part of Blackvoxel.
 *
 * Copyright 2010-2015 Laurent Thiebaut & Olivia Merle
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
 * ZGui_Memory_Monitor.h
 *
 *  Created on: 17 févr. 2015
 *      Author: laurent
 */

#pragma once 

#include "ZGui_Scrolling_Digit.h"
#include "ZGui_PanelTextMultiButton.h"
#include "z/ZCharFilter.h"
#include "ZGui.h"
#include "ZGui_FontFrame.h"
#include "z/ZString.h"
#include "z/ZTypes.h"

class ZGame;

#define ZFRAME_MEMORYMONITOR_DISPLAYLINES 18

class ZVMachine;

class ZFrame_MemoryMonitor : public ZFrame
{
    ZGame * GameEnv;

    ZMemSize MemoryAddress;
    ZFrame_ScrollingDigit Digits[8];
    ZFrame_FontFrame      MemLines[ZFRAME_MEMORYMONITOR_DISPLAYLINES];
    ZPanelTextMultiButton DispSize[3];
    ZPanelTextMultiButton * DispSize_List[4];
    ZVMachine * VMachine;

    ZString               Text_DispSize[3];
    ZString               Text_MemLines[ZFRAME_MEMORYMONITOR_DISPLAYLINES];
    ULong                 DisplaySize;

    ZCharFilter           CharFilter;

  public:

    ZFrame_MemoryMonitor ()
    {
      ULong i;

      FrameType = MulticharConst('M','M','O','N');
      GameEnv = nullptr;
      MemoryAddress = 0;
      VMachine = nullptr;
      for (i=0;i<3;i++) DispSize_List[i] = &DispSize[i];
      DispSize_List[3]=nullptr;
      Text_DispSize[0]="B";
      Text_DispSize[1]="W";
      Text_DispSize[2]="L";
      DisplaySize = 0;
      CharFilter.Fill(0,31,'.');
      CharFilter.Fill(126,254,'.');
    }

    virtual void AddedToFrameCallback(ZFrame * Frame);
    virtual void GetEffectiveHeight(float & Height);
    void         SetGameEnv(ZGame * GameEnv)
    {
      ULong i;

      this->GameEnv = GameEnv;
      for (i=0;i<8;i++) Digits[i].SetGameEnv(GameEnv);
      this->GameEnv = GameEnv;
    }

    void SetVMachine(ZVMachine * VMachine) {this->VMachine = VMachine;}

    virtual void SetSize    (float Width, float Height);
    virtual void Render(Frame_Dimensions * ParentPosition);

};
