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
 * ZGameWindow_Storage.h
 *
 *  Created on: 30 sept. 2011
 *      Author: laurent
 */

#pragma once 

#include "ZGui.h"
#include "ZGui_InventoryBox.h"
#include "ZGui_FontFrame.h"
#include "ZGui_TileFrame.h"
#include "ZVoxelExtension_Storage.h"
#include "z/ZString.h"
#include "z/ZTypes.h"

class ZGame;
class ZVoxelExtension;

class ZGameWindow_Storage : public ZFrame
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
    ZFrame_FontFrame StorageTitle;
    ZFrame_FontFrame InventoryTitle;
    ZTileFrame       Arrow_Transfert;

    ZInventoryBox * MainStorage;

    ZString Text_MainTitle;
    ZString Text_StorageTitle;
    ZString Text_InventoryTitle;

    ZVoxelExtension_Storage * VoxelExtension_Storage;

    UShort i1,i2,i3;
    ULong Q1,Q2,Q3;
  public:

  ZGameWindow_Storage()
  {
    MainWindow = this;
    MainStorage = new ZInventoryBox[128];
    Flag_Shown = false;
    Text_MainTitle = "STORAGE";
    Text_StorageTitle = "STORAGE";
    Text_InventoryTitle = "INVENTORY";
    VoxelExtension_Storage = nullptr;
    i1 = i2 = i3 = 0;
    Q1 = Q2 = Q3 = 0;
    GameEnv = nullptr;
  }

  ~ZGameWindow_Storage()
  {
    if (MainStorage) delete [] MainStorage;
  }

  void SetGameEnv(ZGame * GameEnv) {this->GameEnv = GameEnv;}

  void SetVoxelExtension(ZVoxelExtension * Extension) { VoxelExtension_Storage = (ZVoxelExtension_Storage *)Extension; }

  void Show();
  void Hide();


  bool Is_Shown() {return(Flag_Shown);}

  Bool MouseButtonClick(UShort nButton, Short Absolute_x, Short Absolute_y);


};
