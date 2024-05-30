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
 * ZCircuit.h
 *
 *  Created on: 8 nov. 2014
 *      Author: laurent
 */

#pragma once

#include "z/ZTypes.h"

#include "ZVMachine.h"

class ZChip
{
  public:
    ZVMachine * VMachine;

    inline void  SetVMachine(ZVMachine * VMachine) { this->VMachine = VMachine; }

    inline void  SetRegister(ULong Register, ULong Value) {};
    inline ULong GetRegister(ULong Register) { return(0); };
    void         Reset() {}
    inline void  DoChipManagement() {};
    inline void  DoChipManagement_FrameCycle() {}
    virtual bool Save(ZStream_SpecialRamStream * Stream) {return(false);};
    virtual bool Load(ZStream_SpecialRamStream * Stream) {return(false);};
    virtual ~ZChip() {}
};