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
 * ZGui_VoxelView.h
 *
 *  Created on: 29 févr. 2012
 *      Author: laurent
 */

#pragma once 

#include "ZGui.h"

#include "ZVoxelType.h"

#include "ZVoxelTypeManager.h"

#include "ZTileSets.h"

class ZVoxelViewBox : public ZFrame
{
    UShort   * VoxelType;
    ULong    * Quantity;
    ZVoxelTypeManager * VoxelTypeManager;
//    ZTileStyle * TileStyle;


  public:
    ZVoxelViewBox ()
    {
      FrameType = MulticharConst('V','O','X','V'); // = VoxelViewBox;
      Flag_Cap_Dragable = false;
      VoxelType = 0;
    }

    virtual void SetVoxelType( UShort * VoxelType ) { this->VoxelType = VoxelType; }
//    virtual void SetTileStyle( ZTileStyle * Style ) { this->TileStyle = Style; }
    virtual UShort * GetVoxelType () { return(VoxelType);}
    virtual ULong  * GetQuantity()  { return(Quantity);}
    virtual void SetVoxelTypeManager(ZVoxelTypeManager * VoxelTypeManager) { this->VoxelTypeManager = VoxelTypeManager; }

    virtual void Render(Frame_Dimensions * ParentPosition);

    virtual void DropItem(ZFrame * Item, UShort nButton);


};
