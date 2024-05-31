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
 * ZGame_GUI.h
 *
 *  Created on: 2 juil. 2011
 *      Author: laurent
 */

#pragma once 

#include "ZGui.h"
#include "ZGui_InventoryBox.h"
#include "ZGui_FontFrame.h"
#include "ZGui_TileFrame.h"
#include "z/ZString.h"
#include "z/ZTypes.h"

class ZGame;

class ZGameWindow_Inventory : public ZFrame
{
  protected:
    ZFrame *      MainWindow;
    ZInventoryBox IBox;
    ZInventoryBox IBox2;
    ZInventoryBox IBox3;
    ZGame * GameEnv;
    bool Flag_Shown;

    ZTileFrame       CloseBox;
    ZFrame_FontFrame MainTitle;
    ZFrame_FontFrame ToolTitle;
    ZFrame_FontFrame InventoryTitle;
    ZFrame_FontFrame PowerTitle;
    ZInventoryBox * MainInventory;

    ZString Text_MainTitle;
    ZString Text_ToolTitle;
    ZString Text_InventoryTitle;
    ZString Text_PowerTitle;


    UShort i1,i2,i3;
    ULong Q1,Q2,Q3;
  public:

  ZGameWindow_Inventory()
  {
    GameEnv = nullptr;
    MainWindow = this;
    MainInventory = new ZInventoryBox[128];
    Flag_Shown = false;
    Text_MainTitle = "INVENTORY";
    Text_ToolTitle = "TOOLS";
    Text_InventoryTitle = "INVENTORY";
    Text_PowerTitle = "POWERS";
    i1 = i2 = i3 = 0;
    Q1 = Q2 = Q3 = 0;
  }

  ~ZGameWindow_Inventory()
  {
    if (MainInventory) delete [] MainInventory;
  }

  void SetGameEnv(ZGame * GameEnv) {this->GameEnv = GameEnv;}

  void Show();


  void Hide();
  bool Is_Shown() {return(Flag_Shown);}

  Bool MouseButtonClick(UShort nButton, Short Absolute_x, Short Absolute_y);

};
