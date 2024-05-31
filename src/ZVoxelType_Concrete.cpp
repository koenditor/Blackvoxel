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
 * ZVoxelType_Concrete.cpp
 *
 *  Created on: 16 mars 2014
 *      Author: laurent
 */

#include "ZVoxelType_Concrete.h"

#include "ZVoxelSector.h"
#include "ZVoxelExtensionType_VoxelFluid.h"
#include "z/ZString.h"

class ZVoxelExtension;

ZVoxelExtension * ZVoxelType_Concrete::CreateVoxelExtension(bool IsLoadingPhase)
{
  union
  {
    float             Pressure;
    ZVoxelExtension * Extension;
  };

  Extension = nullptr;
  Pressure  = 0.0;

  return( Extension );

}

void  ZVoxelType_Concrete::GetBlockInformations(ZVoxelLocation * DestLocation, ZString & Infos)
{
  ZVoxelExtensionType_VoxelFluid * VoxelInfo;
  UShort   Pressure;

  VoxelInfo = (ZVoxelExtensionType_VoxelFluid *)&DestLocation->Sector->OtherInfos[DestLocation->Offset];

  Pressure = VoxelInfo->Pressure;

  Infos << "Pressure : " << Pressure;

  return;
}
