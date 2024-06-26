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
 * ZGui_ProgressBar.h
 *
 *  Created on: 22 oct. 2011
 *      Author: laurent
 */

#pragma once 

#include "ZGui.h"
#include "z/ZTypes.h"

class ZTileSet;

class ZFrame_ProgressBar : public ZFrame
{
    ZTileSet * TileSet;
    float Factor;

  public:
    ZFrame_ProgressBar ()
    {
      FrameType = MulticharConst('P','B','A','R'); // = InventoryBox;
      Flag_Cap_Dragable = false;
      Factor = 0.0f;
    }

    void SetCompletion(float Completion) {this->Factor = Completion;}
    virtual void SetTileSet( ZTileSet * TileSet ) { this->TileSet = TileSet; }
    virtual void Render(Frame_Dimensions * ParentPosition);

};
