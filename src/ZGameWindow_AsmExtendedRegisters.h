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
 * ZGameWindow_AsmExtendedRegisters.h
 *
 *  Created on: 1 févr. 2015
 *      Author: laurent
 */

#pragma once 

#include "ZGui.h"
#include "ZGui_CPU_Registers_Extended.h"
#include "ZGui_FontFrame.h"
#include "ZGui_TileFrame.h"
#include "ZGui_ProgressBar.h"
#include "ZVoxelExtension_ProgRobot_Asm.h"
#include "ZGui_Memory_Monitor.h"
#include "z/ZString.h"
#include "z/ZTypes.h"

class ZGame;
class ZVoxelExtension;

class ZGameWindow_AsmExtendedRegisters : public ZFrame
{
  protected:
    ZFrame           * MainWindow;
    ZTileFrame         CloseBox;
    ZFrame_FontFrame   Title;
    ZFrame_FontFrame   Title2;
    ZCPURegisters_Extended RegisterDisplay;
    ZString            Text_Title;
    ZString            Text_Title2;

    ZFrame_MemoryMonitor MemoryMonitor;

    ZFrame_FontFrame   Servo_x_Value;
    ZString            Servo_x_Value_Text;
    ZFrame_ProgressBar Servo_x_Position;

    ZGame              * GameEnv;
    ZVoxelExtension_ProgRobot_Asm * VoxelExtension;
    bool    Flag_Shown;
  public:

  ZGameWindow_AsmExtendedRegisters()
  {
    MainWindow = this;
    GameEnv = nullptr;
    Flag_Shown = false;
    VoxelExtension = nullptr;
    Text_Title       = "REGISTERS";
    Text_Title2      = "MEMORY MONITOR";
    Servo_x_Value_Text = "10000";
  }

  ~ZGameWindow_AsmExtendedRegisters()
  {

  }

  void SetGameEnv(ZGame * GameEnv) {this->GameEnv = GameEnv;}
  void SetVoxelExtension(ZVoxelExtension * Extension) { VoxelExtension = (ZVoxelExtension_ProgRobot_Asm *)Extension; }
  ZVoxelExtension * GetVoxelExtension() { return(VoxelExtension);}

  void Show();
  void Hide();
  bool Is_Shown() {return(Flag_Shown);}

  virtual void Render(Frame_Dimensions * ParentPosition);


  // Overloaded events

  virtual Bool MouseButtonClick  (UShort nButton, Short Absolute_x, Short Absolute_y);



};
