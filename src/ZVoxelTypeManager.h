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
 * ZVoxelTypeManager.h
 *
 *  Created on: 3 oct. 2011
 *      Author: laurent
 */

#pragma once 

#include "z/ZTypes.h"

#include "z/ZObject.h"

#include "ZVoxelType.h"

#include "z/ZFastBit_Array.h"

#include "ZFabMachineInfos.h"

#include "ZFabMachineInfos2.h"



class ZGame;

class ZVoxelTypeManager : public ZObject
{
  ZGame * GameEnv;

  ULong LoadedTexturesCount;

  public:

    ZVoxelType ** VoxelTable;
    ZFastBit_Array_64k * ActiveTable;

    // Constructor

    ZVoxelTypeManager();
    virtual ~ZVoxelTypeManager();

    // Init and fill tables

    void SetGameEnv(ZGame * GameEnv) { this->GameEnv = GameEnv; }
    Bool LoadVoxelTypes();
    void AddVoxelType(UShort TypeNum, ZVoxelType * VoxelType);
    void FillZeroSlots(UShort VoxelTypeUsedToFill);

    // Get information on voxel types

    ZVoxelType * GetVoxelType(UShort TypeNum) { return( VoxelTable[TypeNum] ); }

    ULong GetTexturesCount() { return(LoadedTexturesCount); }

    // Utilities

    void DeleteVoxelExtension(UShort VoxelType, ZMemSize VoxelExtension);

    // Information dumps

    void DumpInfos();
    void OutFabInfos();

    bool _Internal_CompareTransTables( ZString & Message, ZFabInfos * Fab,  ZFabInfos::ZTransformation * Tr1,  ZFabInfos::ZTransformation * Tr2 );
    bool _Internal_CompareTransTables_V2( ZString & Message, ZFabInfos * Fab,  ZFabInfos::ZTransformation * Tr1,  ZFabInfos::ZTransformation * Tr2 );

    void FindFabConflics();
    void FindFabConflicts_V2();

};