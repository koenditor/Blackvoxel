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
 * ZGameWindow_Programmable.h
 *
 *  Created on: 4 janv. 2013
 *      Author: laurent
 */

#pragma once 

#include "ZGui.h"

#include "ZGui_InventoryBox.h"

#include "ZGui_FontFrame.h"

#include "ZGui_NumericChoiceBox.h"

#include "ZGui_PanelTextButton.h"

#include "ZVoxelTypeManager.h"

#include "ZVoxelExtension_Programmable.h"

class ZGame;

class ZGameWindow_Programmable : public ZFrame
{
  protected:
    ZFrame         * MainWindow;
    ZTileFrame       CloseBox;
    ZInventoryBox    IBox;
    ZInventoryBox    IBox2;
    ZInventoryBox    IBox3;
    ZGame          * GameEnv;
    bool             Flag_Shown;

    ZFrame_FontFrame MainTitle;
    ZFrame_FontFrame RobotProgramTitle;
    ZFrame_NumericChoiceBox RobotProgramNum;
    ZPanelTextButton RobotProgramEdit;
    ZPanelTextButton RobotProgramLoad;
    ZFrame_FontFrame ProgramName;
    ZFrame_FontFrame StorageTitle;
    ZFrame_FontFrame InventoryTitle;

    ZInventoryBox * MainStorage;

    ZString Text_MainTitle;
    ZString Text_RobotZoneTitle;
    ZString Text_StorageTitle;
    ZString Text_InventoryTitle;
    ZString Text_RobotProgramEdit;
    ZString Text_RobotProgramLoad;
    ZString Text_ProgramName;
    double  ProgramNum;

    ZVoxelExtension_Programmable * VoxelExtension;

    UShort i1,i2,i3;
    ULong Q1,Q2,Q3;
  public:

  ZGameWindow_Programmable()
  {
    MainWindow = this;
    GameEnv = 0;
    VoxelExtension = 0;
    i1=i2=i3 = 0;
    Q1=Q2=Q3 = 0;
    MainStorage = new ZInventoryBox[128];
    Flag_Shown = false;
    Text_MainTitle = "ROBOT";
    Text_RobotZoneTitle = "ROBOT PROGRAM";
    Text_StorageTitle = "ROBOT STORAGE";
    Text_InventoryTitle = "INVENTORY";
    Text_RobotProgramEdit = "EDIT";
    Text_RobotProgramLoad = "LOAD / COMPILE";
    ProgramNum = 0;
  }

  ~ZGameWindow_Programmable()
  {
    if (MainStorage) delete [] MainStorage;
  }

  void SetGameEnv(ZGame * GameEnv) {this->GameEnv = GameEnv;}

  void SetVoxelExtension(ZVoxelExtension * Extension) { VoxelExtension = (ZVoxelExtension_Programmable *)Extension; }
  ZVoxelExtension * GetVoxelExtension() { return(VoxelExtension);}
  void Show();
  void Hide();


  bool Is_Shown() {return(Flag_Shown);}

  // Overloaded events

  virtual Bool MouseButtonClick  (UShort nButton, Short Absolute_x, Short Absolute_y);

  protected:

    void DisplayProgramName();

};
