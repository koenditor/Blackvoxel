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
 * ZVoxelExtension_Programmable.h
 *
 *  Created on: 28 déc. 2012
 *      Author: laurent
 */

#pragma once 






#include "ZVoxelExtension.h"
#include "ZScripting_Squirrel3.h"
#include "z/ZString.h"
#include "z/ZTypes.h"

template <typename Type> class ZGenericCanva;
class ZStream_SpecialRamStream;
class ZVector3L;

class ZVoxelExtension_Programmable : public ZVoxelExtension
{
  public:
    enum {Storage_NumSlots = 20, ImageTable_Size = 64};
    enum {CONTEXT_NEWVOXEL, CONTEXT_LOADING, CONTEXT_PROGRAMCHANGE};

    UShort  VoxelType[Storage_NumSlots];
    ULong   VoxelQuantity[Storage_NumSlots];
    ULong   RobotLevel;
    ULong   RobotSerialNumber;
    ULong   ScriptNum;
    ULong   Overclock;
    ZString ProgramText;

    bool    IsAllowedToRun;
    // Canvas

    ULong    ImageInUse;
    ZGenericCanva<ULong> * ImageTable[ImageTable_Size];

    // Squirrel script engine

    ZScripting_Squirrel3 Script_Engine;

    virtual ZVoxelExtension * GetNewCopy()
    {
      return nullptr;
    }

    ZVoxelExtension_Programmable()
    {
      ULong i;
      ExtensionType = Extension_Programmable;
      for (i=0;i<Storage_NumSlots;i++) {VoxelType[i] = 0; VoxelQuantity[i] = 0;}
      ScriptNum = 0;
      RobotLevel = 3;
      RobotSerialNumber = 0;
      IsAllowedToRun = false;
      ImageInUse = 0;
      Overclock = 0;
      for (i=0;i<ImageTable_Size;i++) ImageTable[i] = nullptr;
    }

    virtual ~ZVoxelExtension_Programmable();

    void Cleanup()
    {
      Script_Engine.Cleanup();
    }



    virtual ULong GetExtensionID()
    {
      return( MulticharConst('P','R','O','G') );
    }

    virtual bool Save(ZStream_SpecialRamStream * Stream);
    virtual bool Load(ZStream_SpecialRamStream * Stream);


    virtual void SetVoxelPosition(ZVector3L * VoxelPosition)
    {
      Script_Engine.SetVoxelPosition(VoxelPosition);
    }

    virtual bool FindSlot(UShort VoxelType, ULong &Slot)
    {
      ULong i;

      for(i=0;i<Storage_NumSlots;i++)
      {
        if (this->VoxelType[i] == VoxelType && VoxelQuantity[i]>0) { Slot = i; return(true); }
      }
      return(false);
    }

    virtual bool FindFreeSlot(ULong &Slot)
    {
      ULong i;

      for(i=0;i<Storage_NumSlots;i++)
      {
        if (VoxelQuantity[i] == 0) {Slot = i; return(true);}
      }

      return(false);
    }

    virtual ULong FindFirstUsedBlock()
    {
      ULong i;

      for(i=0;i<Storage_NumSlots;i++)
      {
        if (this->VoxelType[i] !=0 && VoxelQuantity[i]>0) { return(i); }
      }
      return(-1);
    }


    virtual ULong StoreBlocks(UShort VoxelType, ULong VoxelQuantity)
    {
      ULong Slot;

      if (FindSlot(VoxelType,Slot))
      {
        this->VoxelQuantity[Slot] += VoxelQuantity;
        return(VoxelQuantity);
      }
      else if (FindFreeSlot(Slot))
      {
        this->VoxelType[Slot]     = VoxelType;
        this->VoxelQuantity[Slot] = VoxelQuantity;
        return(true);
      }

      return(false);
    }

    virtual ULong UnstoreBlocks(ULong SlotNum, ULong VoxelQuantity, UShort * VoxelType)
    {
      ULong Quantity;

      if (SlotNum>=Storage_NumSlots) return(0);
      if (this->VoxelType[SlotNum]==0 || this->VoxelQuantity[SlotNum]<1 ) return(0);
      *VoxelType = this->VoxelType[SlotNum];
      if (this->VoxelQuantity[SlotNum] > VoxelQuantity) { this->VoxelQuantity[SlotNum]-=VoxelQuantity; return(VoxelQuantity);}
      Quantity = this->VoxelQuantity[SlotNum];
      this->VoxelQuantity[SlotNum]=0;
      this->VoxelType[SlotNum]=0;
      return(Quantity);
    }

    virtual void SetScriptNum(ULong ScriptNum) {this->ScriptNum = ScriptNum; }
    virtual void SetSerial(ULong SerialNumber) {this->RobotSerialNumber = SerialNumber; }
    virtual void CompileAndRunScript(ULong Context, ULong ScriptToRun = -1);
    virtual bool IsCompiledOk() { return(Script_Engine.IsCompiledOk()); }

    virtual void StopProgram();
    virtual bool IsRunningProgram();
};
