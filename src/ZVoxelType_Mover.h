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
 * ZVoxelType_Mover.h
 *
 *  Created on: 27 mai 2013
 *      Author: laurent
 */

#pragma once

#include "ZVoxelType.h"
#include "z/ZTypes.h"

class ZVoxelExtension;

class ZVoxelType_Mover : public ZVoxelType
{
  public:
    ZVoxelType_Mover(ULong VoxelType) : ZVoxelType(VoxelType)
    {
      Is_VoxelExtension = true;
      Is_HasAllocatedMemoryExtension = false;
    }

    // Voxel Extensions

    virtual ZVoxelExtension * CreateVoxelExtension(bool IsLoadingPhase = true);


    // The user click the activation button(Middle) on the voxel to activate special functions.
    // virtual void  UserAction_Activate(ZMemSize VoxelInfo, Long x, Long y, Long z);
};
