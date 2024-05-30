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
 * ZGameWindow_AsmHardware.h
 *
 *  Created on: 24 déc. 2014
 *      Author: laurent
 */

#pragma once 

#include "ZGui.h"

#include "ZGui_Disassembler.h"

#include "ZGui_CPU_Registers.h"

#include "ZGui_AsmRobotDisplay.h"

#include "ZGui_PanelTextButton.h"

#include "ZGui_FontFrame.h"

#include "ZGui_TileFrame.h"

#include "ZGui_ProgressBar.h"

#include "ZVoxelExtension_ProgRobot_Asm.h"

class ZGame;

class ZGameWindow_AsmHardware : public ZFrame
{
  protected:
    ZFrame           * MainWindow;
    ZTileFrame         CloseBox;
    ZFrame_FontFrame   Title;
    ZAsmRobotDisplay   RobotDisplay;
    ZString            Text_Title;

    ZFrame_FontFrame   Servo_x_Value;
    ZString            Servo_x_Value_Text;
    ZFrame_ProgressBar Servo_x_Position;

    ZGame              * GameEnv;
    ZVoxelExtension_ProgRobot_Asm * VoxelExtension;
    bool    Flag_Shown;
  public:

  ZGameWindow_AsmHardware()
  {
    MainWindow = this;
    GameEnv = 0;
    Flag_Shown = false;
    VoxelExtension = 0;
    Text_Title       = "ROBOT HARDWARE";
    Servo_x_Value_Text = "10000";
  }

  ~ZGameWindow_AsmHardware()
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
