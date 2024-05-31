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
 * ZGui_InventoryBox.h
 *
 *  Created on: 4 juil. 2011
 *      Author: laurent
 */

#pragma once 

#include "z/ZTypes.h"
#include "ZGui.h"

class ZTileStyle;
class ZVoxelTypeManager;

class ZInventoryBox : public ZFrame
{
  protected:
    UShort   * VoxelType;
    ULong    * Quantity;
    ZVoxelTypeManager * VoxelTypeManager;
    ZTileStyle * TileStyle;
    ZTileStyle * TileStyle_Reduced;
    ZMemSize   ReducedFontThreshold;

  public:
    ZInventoryBox ()
    {
      FrameType = MulticharConst('I','B','O','X'); // = InventoryBox;
      Flag_Cap_Dragable = true;
      VoxelType = nullptr;
      Quantity = nullptr;
      VoxelTypeManager = nullptr;
      TileStyle = nullptr;
      TileStyle_Reduced = nullptr;
      ReducedFontThreshold = 4;
    }

    enum FontType {FONTTYPE_MAIN=0, FONTTYPE_REDUCED=1};

    virtual void SetVoxelType( UShort * VoxelType ) { this->VoxelType = VoxelType; }
    virtual void SetQuantity( ULong * Quantity )    { this->Quantity = Quantity;   }
    virtual void SetTileStyle( ZTileStyle * Style, FontType TyleType=FONTTYPE_MAIN)
    {
      switch(TyleType)
      {
        default:
        case FONTTYPE_MAIN: this->TileStyle = Style; break;
        case FONTTYPE_REDUCED: this->TileStyle_Reduced = Style; break;
      }
    }
    virtual void SetReducedFontThreshold(ZMemSize ReducedFontThreshold=4) {this->ReducedFontThreshold = ReducedFontThreshold;}
    virtual UShort * GetVoxelType () { return(VoxelType);}
    virtual ULong  * GetQuantity()  { return(Quantity);}
    virtual void SetVoxelTypeManager(ZVoxelTypeManager * VoxelTypeManager) { this->VoxelTypeManager = VoxelTypeManager; }

    virtual void Render(Frame_Dimensions * ParentPosition);

    virtual void DropItem(ZFrame * Item, UShort nButton);


};
