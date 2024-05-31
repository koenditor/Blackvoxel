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
 * ZGui_TileFrame.h
 *
 *  Created on: 1 juin 2011
 *      Author: laurent
 */

#pragma once 

#include "ZGui.h"
#include "z/ZTypes.h"

class ZTileSet;

class ZTileFrame : public ZFrame
{
    ZTileSet * TileSet;
    UByte    Tile;
  public:
    ZTileFrame () {FrameType = MulticharConst('T','I','L','E'); TileSet = nullptr; Tile=0;}


    virtual void SetTileSet( ZTileSet * TileSet ) { this->TileSet = TileSet; }
    virtual void SetTile( UByte Tile ) { this->Tile = Tile; }

    virtual void Render(Frame_Dimensions * ParentPosition);
};
